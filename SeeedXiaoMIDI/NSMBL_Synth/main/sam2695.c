#include "sam2695.h"
#include "config.h"

#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "sam2695";

static void send_bytes(const uint8_t *data, size_t len)
{
    uart_write_bytes(SAM_UART_NUM, data, len);
}

esp_err_t sam2695_init(void)
{
    uart_config_t uart_config = {
        .baud_rate  = SAM_UART_BAUD,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_param_config(SAM_UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(SAM_UART_NUM, SAM_TX_PIN, SAM_RX_PIN,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(SAM_UART_NUM, 256, 256, 0, NULL, 0));

    // SAM2695 needs ~500-600ms after power-up
    vTaskDelay(pdMS_TO_TICKS(600));

    ESP_LOGI(TAG, "SAM2695 UART initialized (baud=%d, tx=%d, rx=%d)",
             SAM_UART_BAUD, SAM_TX_PIN, SAM_RX_PIN);
    return ESP_OK;
}

void sam2695_note_on(uint8_t channel, uint8_t note, uint8_t velocity)
{
    uint8_t msg[] = { (uint8_t)(0x90 | (channel & 0x0F)), note & 0x7F, velocity & 0x7F };
    send_bytes(msg, 3);
}

void sam2695_note_off(uint8_t channel, uint8_t note)
{
    uint8_t msg[] = { (uint8_t)(0x80 | (channel & 0x0F)), note & 0x7F, 0x00 };
    send_bytes(msg, 3);
}

void sam2695_program_change(uint8_t channel, uint8_t bank, uint8_t program)
{
    // Bank Select MSB must precede Program Change
    uint8_t bank_msg[] = { (uint8_t)(0xB0 | (channel & 0x0F)), 0x00, bank & 0x7F };
    send_bytes(bank_msg, 3);

    uint8_t pc_msg[] = { (uint8_t)(0xC0 | (channel & 0x0F)), program & 0x7F };
    send_bytes(pc_msg, 2);
}

void sam2695_control_change(uint8_t channel, uint8_t cc, uint8_t value)
{
    uint8_t msg[] = { (uint8_t)(0xB0 | (channel & 0x0F)), cc & 0x7F, value & 0x7F };
    send_bytes(msg, 3);
}

void sam2695_pitch_bend(uint8_t channel, uint16_t bend)
{
    uint8_t lsb = bend & 0x7F;
    uint8_t msb = (bend >> 7) & 0x7F;
    uint8_t msg[] = { (uint8_t)(0xE0 | (channel & 0x0F)), lsb, msb };
    send_bytes(msg, 3);
}

void sam2695_aftertouch(uint8_t channel, uint8_t pressure)
{
    uint8_t msg[] = { (uint8_t)(0xD0 | (channel & 0x0F)), pressure & 0x7F };
    send_bytes(msg, 2);
}

void sam2695_all_notes_off(uint8_t channel)
{
    sam2695_control_change(channel, 120, 0);  // All Sound Off
    sam2695_control_change(channel, 123, 0);  // All Notes Off
}

void sam2695_set_volume(uint8_t channel, uint8_t volume)
{
    sam2695_control_change(channel, 7, volume);
}

void sam2695_boot_sound(uint8_t channel)
{
    // Signature beat: kick-hat-snare-hat-kick-kick-crash
    sam2695_note_on(channel, 36, 127);
    vTaskDelay(pdMS_TO_TICKS(150));
    sam2695_note_off(channel, 36);

    sam2695_note_on(channel, 42, 90);
    vTaskDelay(pdMS_TO_TICKS(150));
    sam2695_note_off(channel, 42);

    sam2695_note_on(channel, 38, 127);
    vTaskDelay(pdMS_TO_TICKS(150));
    sam2695_note_off(channel, 38);

    sam2695_note_on(channel, 42, 90);
    vTaskDelay(pdMS_TO_TICKS(150));
    sam2695_note_off(channel, 42);

    sam2695_note_on(channel, 36, 127);
    vTaskDelay(pdMS_TO_TICKS(100));
    sam2695_note_off(channel, 36);
    sam2695_note_on(channel, 36, 100);
    vTaskDelay(pdMS_TO_TICKS(150));
    sam2695_note_off(channel, 36);

    sam2695_note_on(channel, 49, 100);
    vTaskDelay(pdMS_TO_TICKS(400));
    sam2695_note_off(channel, 49);
}
