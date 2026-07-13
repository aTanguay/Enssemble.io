#include "oled.h"
#include "config.h"

#include "u8g2.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <string.h>
#include <stdio.h>

static const char *TAG = "oled";

#define DISP_W          128
#define DISP_H          128
#define HEADER_H        18
#define ROW_H           14      // 6x13 font + 1px padding

static u8g2_t s_u8g2;
static i2c_master_dev_handle_t s_dev;
static bool s_present = false;

// One I2C transfer's worth of bytes, buffered between START/END_TRANSFER.
static uint8_t s_tx[256];
static int s_tx_len;

// u8g2 byte callback: the SH1107 cad layer has already inserted the 0x00/0x40
// control bytes, so we just accumulate the framed stream and ship it.
static uint8_t oled_byte_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg) {
    case U8X8_MSG_BYTE_INIT:
        return 1;
    case U8X8_MSG_BYTE_START_TRANSFER:
        s_tx_len = 0;
        return 1;
    case U8X8_MSG_BYTE_SEND: {
        uint8_t *d = (uint8_t *)arg_ptr;
        for (int i = 0; i < arg_int; i++) {
            if (s_tx_len < (int)sizeof(s_tx)) s_tx[s_tx_len++] = d[i];
        }
        return 1;
    }
    case U8X8_MSG_BYTE_END_TRANSFER:
        if (s_dev && s_tx_len > 0) {
            i2c_master_transmit(s_dev, s_tx, s_tx_len, 100);
        }
        return 1;
    default:
        return 1;
    }
}

static uint8_t oled_gpio_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
        return 1;
    case U8X8_MSG_DELAY_MILLI:
        vTaskDelay(pdMS_TO_TICKS(arg_int));
        return 1;
    case U8X8_MSG_DELAY_10MICRO:
        esp_rom_delay_us(10);
        return 1;
    case U8X8_MSG_DELAY_100NANO:
        return 1;
    default:
        return 1;
    }
}

esp_err_t oled_init(i2c_master_bus_handle_t bus)
{
    if (i2c_master_probe(bus, OLED_ADDR, 50) != ESP_OK) {
        ESP_LOGW(TAG, "SH1107 not found at 0x%02X — screen disabled", OLED_ADDR);
        return ESP_ERR_NOT_FOUND;
    }

    i2c_device_config_t dcfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = OLED_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
    };
    esp_err_t ret = i2c_master_bus_add_device(bus, &dcfg, &s_dev);
    if (ret != ESP_OK) return ret;

    OLED_SETUP(&s_u8g2, U8G2_R0, oled_byte_cb, oled_gpio_delay_cb);
    u8x8_SetI2CAddress(u8g2_GetU8x8(&s_u8g2), OLED_ADDR << 1);
    u8g2_InitDisplay(&s_u8g2);
    u8g2_SetPowerSave(&s_u8g2, 0);
    u8g2_ClearBuffer(&s_u8g2);
    u8g2_SendBuffer(&s_u8g2);

    s_present = true;
    ESP_LOGI(TAG, "SH1107 128x128 up at 0x%02X", OLED_ADDR);
    return ESP_OK;
}

bool oled_present(void)
{
    return s_present;
}

static void draw_centered(const u8g2_uint_t y, const char *s)
{
    u8g2_uint_t w = u8g2_GetStrWidth(&s_u8g2, s);
    u8g2_uint_t x = (w < DISP_W) ? (DISP_W - w) / 2 : 0;
    u8g2_DrawStr(&s_u8g2, x, y, s);
}

void oled_draw_message(const char *line1, const char *line2)
{
    if (!s_present) return;
    u8g2_ClearBuffer(&s_u8g2);
    u8g2_SetFont(&s_u8g2, u8g2_font_helvB12_tf);
    if (line1) draw_centered(56, line1);
    if (line2) draw_centered(80, line2);
    u8g2_SendBuffer(&s_u8g2);
}

void oled_draw_menu(const char *const *labels, const char *const *vals,
                    const int *bars, int count, int selected)
{
    if (!s_present) return;

    u8g2_ClearBuffer(&s_u8g2);

    // Header
    u8g2_SetFont(&s_u8g2, u8g2_font_helvB12_tf);
    u8g2_DrawStr(&s_u8g2, 2, 14, "SYNTH");
    u8g2_DrawHLine(&s_u8g2, 0, HEADER_H - 1, DISP_W);

    u8g2_SetFont(&s_u8g2, u8g2_font_6x13_tf);
    for (int i = 0; i < count; i++) {
        int row_top = HEADER_H + i * ROW_H;
        int baseline = row_top + 11;
        bool sel = (i == selected);

        if (sel) {
            u8g2_DrawBox(&s_u8g2, 0, row_top, DISP_W, ROW_H);
            u8g2_SetDrawColor(&s_u8g2, 0); // draw this row's content inverted
        }

        u8g2_DrawStr(&s_u8g2, 2, baseline, labels[i]);

        if (bars[i] >= 0) {
            // value bar (0..127) then the number, right-aligned
            const int bx = 60, bw = 40, by = row_top + 3, bh = 8;
            u8g2_DrawFrame(&s_u8g2, bx, by, bw, bh);
            int fill = bars[i] * (bw - 2) / 127;
            if (fill > 0) u8g2_DrawBox(&s_u8g2, bx + 1, by + 1, fill, bh - 2);
        }
        // right-aligned value text (number for params, name for Patch)
        u8g2_uint_t w = u8g2_GetStrWidth(&s_u8g2, vals[i]);
        u8g2_DrawStr(&s_u8g2, DISP_W - 2 - w, baseline, vals[i]);

        if (sel) u8g2_SetDrawColor(&s_u8g2, 1);
    }

    u8g2_SendBuffer(&s_u8g2);
}

void oled_draw_edit(const char *label, const char *val, int bar)
{
    if (!s_present) return;

    u8g2_ClearBuffer(&s_u8g2);

    // Header
    u8g2_SetFont(&s_u8g2, u8g2_font_helvB12_tf);
    u8g2_DrawStr(&s_u8g2, 2, 14, "EDIT");
    u8g2_DrawHLine(&s_u8g2, 0, HEADER_H - 1, DISP_W);

    // Big centered label
    u8g2_SetFont(&s_u8g2, u8g2_font_helvB14_tf);
    draw_centered(56, label);

    if (bar >= 0) {
        // wide value bar + number
        const int bx = 12, bw = 104, by = 72, bh = 18;
        u8g2_DrawFrame(&s_u8g2, bx, by, bw, bh);
        int fill = bar * (bw - 4) / 127;
        if (fill > 0) u8g2_DrawBox(&s_u8g2, bx + 2, by + 2, fill, bh - 4);
        u8g2_SetFont(&s_u8g2, u8g2_font_helvB12_tf);
        draw_centered(112, val);
    } else {
        // Patch name, centered
        u8g2_SetFont(&s_u8g2, u8g2_font_6x13_tf);
        draw_centered(84, val);
    }

    u8g2_SendBuffer(&s_u8g2);
}
