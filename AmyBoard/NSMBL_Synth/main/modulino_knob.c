#include "modulino_knob.h"
#include "config.h"

#include "esp_log.h"

static const char *TAG = "knob";

// Modulino Knob I2C read frame (verified against Arduino_Modulino source). The
// base Module::read() requests howmany+1 bytes and the FIRST byte is the pinstrap
// address, so on the wire we get:
//   byte 0    : pinstrap address (discard)
//   byte 1..2 : int16 LE encoder count
//   byte 3    : button state (non-zero = pressed)
#define KNOB_READ_LEN   4
#define I2C_TIMEOUT_MS  50

static i2c_master_dev_handle_t s_dev;
static bool    s_present = false;
static int16_t s_last_count;
static bool    s_last_pressed;
static bool    s_have_last = false;

static esp_err_t add_at_addr(i2c_master_bus_handle_t bus, uint8_t addr)
{
    if (i2c_master_probe(bus, addr, I2C_TIMEOUT_MS) != ESP_OK) {
        return ESP_ERR_NOT_FOUND;
    }
    i2c_device_config_t dcfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = I2C_FREQ_HZ,
    };
    return i2c_master_bus_add_device(bus, &dcfg, &s_dev);
}

esp_err_t modulino_knob_init(i2c_master_bus_handle_t bus)
{
    const uint8_t addrs[2] = { KNOB_ADDR_PRIMARY, KNOB_ADDR_ALT };
    for (int i = 0; i < 2; i++) {
        if (add_at_addr(bus, addrs[i]) == ESP_OK) {
            s_present = true;
            ESP_LOGI(TAG, "Modulino Knob found at 0x%02X", addrs[i]);
            return ESP_OK;
        }
    }
    ESP_LOGW(TAG, "Modulino Knob not found (0x%02X/0x%02X) — knob UI disabled",
             KNOB_ADDR_PRIMARY, KNOB_ADDR_ALT);
    return ESP_ERR_NOT_FOUND;
}

bool modulino_knob_present(void)
{
    return s_present;
}

esp_err_t modulino_knob_poll(int *delta, bool *pressed_edge)
{
    if (delta) *delta = 0;
    if (pressed_edge) *pressed_edge = false;
    if (!s_present) return ESP_ERR_INVALID_STATE;

    uint8_t buf[KNOB_READ_LEN];
    esp_err_t ret = i2c_master_receive(s_dev, buf, sizeof(buf), I2C_TIMEOUT_MS);
    if (ret != ESP_OK) return ret;

    // buf[0] = pinstrap address (discard); [1..2] = int16 LE count; [3] = button
    int16_t count = (int16_t)(buf[1] | (buf[2] << 8));
    bool pressed = (buf[3] != 0);

    if (!s_have_last) {
        // First read only establishes a baseline (avoids a bogus jump).
        s_last_count = count;
        s_last_pressed = pressed;
        s_have_last = true;
        return ESP_OK;
    }

    // Signed subtraction of int16 is inherently wrap-safe across the 16-bit range.
    if (delta) *delta = (int)(int16_t)(count - s_last_count);
    if (pressed_edge) *pressed_edge = (pressed && !s_last_pressed);

    s_last_count = count;
    s_last_pressed = pressed;
    return ESP_OK;
}
