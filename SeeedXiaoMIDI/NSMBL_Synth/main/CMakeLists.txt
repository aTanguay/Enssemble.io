idf_component_register(
    SRCS "main.c" "ble_midi.c" "sam2695.c" "buttons.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES nvs_flash bt driver
)
