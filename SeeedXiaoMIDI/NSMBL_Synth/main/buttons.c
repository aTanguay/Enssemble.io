#include "buttons.h"
#include "config.h"

#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "buttons";

typedef struct {
    uint8_t  pin;
    bool     pressed;
    uint32_t press_time;
    bool     long_fired;
    int      last_raw;      // last raw GPIO level, for edge logging
} button_state_t;

static button_state_t s_buttons[4];
static button_cb_t s_on_short;
static button_cb_t s_on_long;

static uint32_t now_ms(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000);
}

void buttons_init(button_cb_t on_short, button_cb_t on_long)
{
    s_on_short = on_short;
    s_on_long  = on_long;

    const uint8_t pins[] = { BUTTON_0_PIN, BUTTON_1_PIN, BUTTON_2_PIN, BUTTON_3_PIN };

    for (int i = 0; i < 4; i++) {
        s_buttons[i].pin = pins[i];
        s_buttons[i].pressed = false;
        s_buttons[i].long_fired = false;

        gpio_config_t cfg = {
            .pin_bit_mask = (1ULL << pins[i]),
            .mode         = GPIO_MODE_INPUT,
            .pull_up_en   = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type    = GPIO_INTR_DISABLE,
        };
        gpio_config(&cfg);
        s_buttons[i].last_raw = gpio_get_level(pins[i]);
    }

    ESP_LOGI(TAG, "Buttons init: pins %d/%d/%d/%d, resting levels %d/%d/%d/%d (1=released)",
             pins[0], pins[1], pins[2], pins[3],
             s_buttons[0].last_raw, s_buttons[1].last_raw,
             s_buttons[2].last_raw, s_buttons[3].last_raw);
}

void buttons_poll(void)
{
    uint32_t t = now_ms();

    // Heartbeat: confirm the poll loop runs even while BLE is busy
    static uint32_t last_hb = 0;
    if (t - last_hb >= 3000) {
        last_hb = t;
        ESP_LOGI(TAG, "poll alive; raw levels %d/%d/%d/%d",
                 gpio_get_level(s_buttons[0].pin), gpio_get_level(s_buttons[1].pin),
                 gpio_get_level(s_buttons[2].pin), gpio_get_level(s_buttons[3].pin));
    }

    for (int i = 0; i < 4; i++) {
        int raw = gpio_get_level(s_buttons[i].pin);
        if (raw != s_buttons[i].last_raw) {
            s_buttons[i].last_raw = raw;
            ESP_LOGI(TAG, "BTN %d (pin %d) -> %d", i, s_buttons[i].pin, raw);
        }

        bool down = (raw == 0);

        if (down && !s_buttons[i].pressed) {
            s_buttons[i].pressed    = true;
            s_buttons[i].press_time = t;
            s_buttons[i].long_fired = false;
        }
        else if (!down && s_buttons[i].pressed) {
            s_buttons[i].pressed = false;
            if (!s_buttons[i].long_fired && (t - s_buttons[i].press_time < LONG_PRESS_MS)) {
                if (s_on_short) s_on_short(i);
            }
        }
        else if (down && s_buttons[i].pressed && !s_buttons[i].long_fired) {
            if (t - s_buttons[i].press_time >= LONG_PRESS_MS) {
                s_buttons[i].long_fired = true;
                if (s_on_long) s_on_long(i);
            }
        }
    }
}
