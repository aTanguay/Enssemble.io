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

void oled_draw_bridge(uint8_t in, uint8_t out, int active_field,
                      const char *note_str, bool in_lit, bool out_lit, uint32_t count)
{
    if (!s_present) return;

    char lin[10], lout[10], cbuf[20];
    if (in == 0)  snprintf(lin, sizeof(lin), "IN:ALL");
    else          snprintf(lin, sizeof(lin), "IN:%u", in);
    if (out == 0) snprintf(lout, sizeof(lout), "OUT:THRU");
    else          snprintf(lout, sizeof(lout), "OUT:%u", out);

    u8g2_ClearBuffer(&s_u8g2);

    // --- Header: IN  ->  OUT, active field boxed ---
    u8g2_SetFont(&s_u8g2, u8g2_font_6x13_tf);
    u8g2_DrawStr(&s_u8g2, 2, 12, lin);
    u8g2_DrawStr(&s_u8g2, 58, 12, "\xbb");                 // '»' arrow (latin-1)
    u8g2_uint_t wout = u8g2_GetStrWidth(&s_u8g2, lout);
    int outx = DISP_W - 2 - wout;
    u8g2_DrawStr(&s_u8g2, outx, 12, lout);
    if (active_field == 0) {
        u8g2_uint_t win = u8g2_GetStrWidth(&s_u8g2, lin);
        u8g2_DrawFrame(&s_u8g2, 0, 0, win + 4, 15);
    } else {
        u8g2_DrawFrame(&s_u8g2, outx - 2, 0, wout + 4, 15);
    }
    u8g2_DrawHLine(&s_u8g2, 0, HEADER_H - 1, DISP_W);

    // --- Big last-note readout ---
    u8g2_SetFont(&s_u8g2, u8g2_font_helvB14_tf);
    draw_centered(58, (note_str && note_str[0]) ? note_str : "--");

    // --- IN / OUT activity dots (filled = recent traffic) ---
    u8g2_SetFont(&s_u8g2, u8g2_font_6x13_tf);
    u8g2_DrawStr(&s_u8g2, 22, 94, "IN");
    if (in_lit) u8g2_DrawDisc(&s_u8g2, 48, 90, 5, U8G2_DRAW_ALL);
    else        u8g2_DrawCircle(&s_u8g2, 48, 90, 5, U8G2_DRAW_ALL);
    u8g2_DrawStr(&s_u8g2, 74, 94, "OUT");
    if (out_lit) u8g2_DrawDisc(&s_u8g2, 108, 90, 5, U8G2_DRAW_ALL);
    else         u8g2_DrawCircle(&s_u8g2, 108, 90, 5, U8G2_DRAW_ALL);

    // --- Forwarded-note counter ---
    snprintf(cbuf, sizeof(cbuf), "notes: %lu", (unsigned long)count);
    draw_centered(116, cbuf);

    u8g2_SendBuffer(&s_u8g2);
}
