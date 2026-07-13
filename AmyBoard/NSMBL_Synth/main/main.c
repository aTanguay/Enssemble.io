#include "config.h"
#include "ble_midi.h"
#include "synth_engine.h"
#include "i2c_bus.h"
#include "ui.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static const char *TAG = "nsmbl";

void app_main(void)
{
    ESP_LOGI(TAG, "=== %s starting ===", DEVICE_NAME);

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    QueueHandle_t midi_queue = xQueueCreate(MIDI_QUEUE_SIZE, sizeof(midi_event_t));

    synth_engine_init(midi_queue);

    // Local UI (I2C encoder + OLED). Non-fatal: if the bus or devices are absent,
    // the board keeps working as a plain BLE synth. UI injects patch/CC events
    // into the same midi_queue the synth task consumes.
    if (i2c_bus_init() == ESP_OK) {
        ui_init(midi_queue, i2c_bus_get());
    }

    ble_midi_init(midi_queue);

    ESP_LOGI(TAG, "=== %s ready ===", DEVICE_NAME);
}
