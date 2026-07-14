#include "config.h"
#include "ble_midi.h"
#include "midi_out.h"
#include "bridge_state.h"
#include "i2c_bus.h"
#include "ui.h"

#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static const char *TAG = "garee";

// --- Bridge loop: BLE MIDI in -> channel remap -> DIN MIDI out ---
static void bridge_task(void *arg)
{
    QueueHandle_t q = (QueueHandle_t)arg;
    midi_event_t event;

    while (1) {
        if (xQueueReceive(q, &event, portMAX_DELAY)) {
            // event.channel is 1-16. Emit on the original channel (passthrough)
            // or remap everything to the runtime OUT channel (set by the UI knob).
            uint8_t out = bridge_out();
            uint8_t out_ch = (out == BRIDGE_PASSTHROUGH) ? event.channel : out;
            midi_out_send_event(&event, (uint8_t)(out_ch - 1));  // to 0-indexed
            bridge_note_out(event.type, event.data1, event.data2);  // OUT activity/monitor
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "=================================");
    ESP_LOGI(TAG, "  %s", DEVICE_NAME);
    ESP_LOGI(TAG, "  AMYboard BLE -> DIN MIDI bridge");
    ESP_LOGI(TAG, "  in=%d (0=all)  out=%d (0=passthrough)", BRIDGE_IN, BRIDGE_OUT);
    ESP_LOGI(TAG, "=================================");

    // NVS (required for BLE)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Runtime routing seeded from the config defaults (UI can change it live)
    bridge_state_init(BRIDGE_IN, BRIDGE_OUT);

    // DIN MIDI output (UART1) — must come up before we forward anything
    midi_out_init();

    // MIDI queue + BLE input
    QueueHandle_t midi_queue = xQueueCreate(MIDI_QUEUE_SIZE, sizeof(midi_event_t));
    ble_midi_init(midi_queue);

    xTaskCreate(bridge_task, "bridge", 4096, midi_queue, 5, NULL);

    // Local UI (I2C encoder + OLED). Non-fatal: if absent, the bridge still
    // forwards using the config-default routing.
    if (i2c_bus_init() == ESP_OK) {
        ui_init(i2c_bus_get());
    }

    ESP_LOGI(TAG, "=== %s ready ===", DEVICE_NAME);
}
