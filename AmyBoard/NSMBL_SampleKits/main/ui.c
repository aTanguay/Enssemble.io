#include "ui.h"
#include "config.h"
#include "modulino_knob.h"
#include "oled.h"
#include "sample_player.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <dirent.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>

static const char *TAG = "ui";

#define UI_NAME_MAX 40
#define UI_PATH_MAX 96

static char  s_paths[MAX_KITS][UI_PATH_MAX];
static char  s_names[MAX_KITS][UI_NAME_MAX];   // basename, no extension
static const char *s_name_ptrs[MAX_KITS];
static int   s_count;

static int   s_selected;   // highlighted row
static int   s_loaded;     // currently-playing kit (-1 if none/unknown)

static void strip_ext(char *s)
{
    char *dot = strrchr(s, '.');
    if (dot) *dot = '\0';
}

static void scan_kits(void)
{
    s_count = 0;
    DIR *d = opendir(KITS_DIR);
    if (!d) {
        ESP_LOGW(TAG, "opendir(%s) failed — no kits to browse", KITS_DIR);
        return;
    }

    struct dirent *e;
    while ((e = readdir(d)) != NULL && s_count < MAX_KITS) {
        const char *n = e->d_name;
        size_t len = strlen(n);
        if (len < 5 || strcasecmp(n + len - 4, ".wav") != 0) continue;

        // %.80s bounds the name so the path provably fits UI_PATH_MAX.
        snprintf(s_paths[s_count], UI_PATH_MAX, "%s/%.80s", KITS_DIR, n);
        strlcpy(s_names[s_count], n, UI_NAME_MAX);
        strip_ext(s_names[s_count]);
        s_count++;
    }
    closedir(d);

    // Alphabetical (case-insensitive) — small N, simple selection sort.
    for (int i = 0; i < s_count; i++) {
        for (int j = i + 1; j < s_count; j++) {
            if (strcasecmp(s_names[j], s_names[i]) < 0) {
                char tp[UI_PATH_MAX], tn[UI_NAME_MAX];
                memcpy(tp, s_paths[i], UI_PATH_MAX); memcpy(s_paths[i], s_paths[j], UI_PATH_MAX); memcpy(s_paths[j], tp, UI_PATH_MAX);
                memcpy(tn, s_names[i], UI_NAME_MAX); memcpy(s_names[i], s_names[j], UI_NAME_MAX); memcpy(s_names[j], tn, UI_NAME_MAX);
            }
        }
    }
    for (int i = 0; i < s_count; i++) {
        s_name_ptrs[i] = s_names[i];
        ESP_LOGI(TAG, "kit %2d: %s", i, s_names[i]);
    }
    ESP_LOGI(TAG, "Found %d kit(s) in %s", s_count, KITS_DIR);
}

// Match the boot-loaded kit (sample_player_current_kit) to a list index.
static int find_loaded_index(void)
{
    const char *cur = sample_player_current_kit();
    for (int i = 0; i < s_count; i++) {
        if (strcmp(s_names[i], cur) == 0) return i;
    }
    return -1;
}

static void load_selected(void)
{
    if (s_selected < 0 || s_selected >= s_count) return;

    ESP_LOGI(TAG, "Loading kit %d: %s", s_selected, s_names[s_selected]);
    oled_draw_loading(s_names[s_selected]);

    if (sample_player_load_kit(s_paths[s_selected]) == ESP_OK) {
        s_loaded = s_selected;
    } else {
        ESP_LOGE(TAG, "load failed: %s", s_paths[s_selected]);
    }
    oled_draw_kit_list(s_name_ptrs, s_count, s_selected, s_loaded);
}

static void ui_task(void *param)
{
    i2c_master_bus_handle_t bus = (i2c_master_bus_handle_t)param;

    modulino_knob_init(bus);
    oled_init(bus);
    scan_kits();

    s_loaded = find_loaded_index();
    s_selected = (s_loaded >= 0) ? s_loaded : 0;

    ESP_LOGI(TAG, "UI task on core %d (%d kits, knob=%d oled=%d)",
             xPortGetCoreID(), s_count, modulino_knob_present(), oled_present());

    oled_draw_kit_list(s_name_ptrs, s_count, s_selected, s_loaded);

    while (1) {
        int delta = 0;
        bool pressed = false;
        modulino_knob_poll(&delta, &pressed);

        if (delta != 0 && s_count > 0) {
            int next = s_selected + delta;
            if (next < 0) next = 0;
            if (next >= s_count) next = s_count - 1;
            if (next != s_selected) {
                s_selected = next;
                oled_draw_kit_list(s_name_ptrs, s_count, s_selected, s_loaded);
            }
        }

        if (pressed && s_count > 0 && s_selected != s_loaded) {
            load_selected();
        }

        vTaskDelay(pdMS_TO_TICKS(20)); // ~50 Hz
    }
}

void ui_init(i2c_master_bus_handle_t bus)
{
    // Low priority on the BLE/control core so it never disturbs audio on core 1.
    xTaskCreatePinnedToCore(ui_task, "ui", 4096, bus, 3, NULL, BLE_CORE);
}
