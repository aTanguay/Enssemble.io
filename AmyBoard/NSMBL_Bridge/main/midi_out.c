#include "midi_out.h"
#include "config.h"

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "midi_out";

static void tx(const uint8_t *data, size_t len)
{
    uart_write_bytes(MIDI_UART_NUM, data, len);
}

void midi_out_init(void)
{
    uart_config_t cfg = {
        .baud_rate  = MIDI_BAUD,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config(MIDI_UART_NUM, &cfg));
    ESP_ERROR_CHECK(uart_set_pin(MIDI_UART_NUM, MIDI_TX_PIN, MIDI_RX_PIN,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(MIDI_UART_NUM, 256, 256, 0, NULL, 0));

    // The AMYboard OUT jack is TRS tip=GPIO14, ring=GPIO15, sleeve=GND with no
    // fixed 3.3V rail. A DIN MIDI input's optocoupler needs a current source, so
    // the non-data leg must be held HIGH (MIDI idle/source) or nothing conducts.
    // (Matches stock tulipcc amyboard_set_midi_out().)
    gpio_reset_pin(MIDI_SRC_PIN);
    gpio_set_direction(MIDI_SRC_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(MIDI_SRC_PIN, 1);

    ESP_LOGI(TAG, "DIN MIDI out: UART%d TX=GPIO%d (src leg GPIO%d high) @ %d baud",
             MIDI_UART_NUM, MIDI_TX_PIN, MIDI_SRC_PIN, MIDI_BAUD);
}

void midi_out_send_event(const midi_event_t *event, uint8_t channel)
{
    uint8_t ch = channel & 0x0F;
    uint8_t buf[3];

    switch (event->type) {
    case MIDI_NOTE_ON:
        buf[0] = 0x90 | ch; buf[1] = event->data1 & 0x7F; buf[2] = event->data2 & 0x7F;
        tx(buf, 3);
        break;
    case MIDI_NOTE_OFF:
        buf[0] = 0x80 | ch; buf[1] = event->data1 & 0x7F; buf[2] = 0x00;
        tx(buf, 3);
        break;
    case MIDI_CC:
        buf[0] = 0xB0 | ch; buf[1] = event->data1 & 0x7F; buf[2] = event->data2 & 0x7F;
        tx(buf, 3);
        break;
    case MIDI_PROGRAM_CHANGE:
        buf[0] = 0xC0 | ch; buf[1] = event->data1 & 0x7F;
        tx(buf, 2);
        break;
    case MIDI_PITCH_BEND: {
        uint16_t bend = (uint16_t)(event->pitch_bend + 8192);
        buf[0] = 0xE0 | ch; buf[1] = bend & 0x7F; buf[2] = (bend >> 7) & 0x7F;
        tx(buf, 3);
        break;
    }
    case MIDI_AFTERTOUCH:
        buf[0] = 0xD0 | ch; buf[1] = event->data1 & 0x7F;
        tx(buf, 2);
        break;
    }
}
