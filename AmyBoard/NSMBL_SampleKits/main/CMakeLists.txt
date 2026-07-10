idf_component_register(
    SRCS "main.c" "ble_midi.c" "synth_engine.c" "sd_card.c" "sample_player.c"
         "i2c_bus.c" "modulino_knob.c" "oled.c" "ui.c"
    INCLUDE_DIRS "."
    REQUIRES bt nvs_flash fatfs driver esp_driver_i2s esp_driver_spi esp_driver_i2c u8g2
)
