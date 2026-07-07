#include "config.h"
#include "ble_midi.h"
#include "midi_out.h"

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
            // or remap everything to BRIDGE_OUT.
            uint8_t out_ch = (BRIDGE_OUT == BRIDGE_PASSTHROUGH) ? event.channel : BRIDGE_OUT;
            midi_out_send_event(&event, (uint8_t)(out_ch - 1));  // to 0-indexed
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

    // DIN MIDI output (UART1) — must come up before we forward anything
    midi_out_init();

    // MIDI queue + BLE input
    QueueHandle_t midi_queue = xQueueCreate(MIDI_QUEUE_SIZE, sizeof(midi_event_t));
    ble_midi_init(midi_queue);

    xTaskCreate(bridge_task, "bridge", 4096, midi_queue, 5, NULL);

    ESP_LOGI(TAG, "=== %s ready ===", DEVICE_NAME);
}
