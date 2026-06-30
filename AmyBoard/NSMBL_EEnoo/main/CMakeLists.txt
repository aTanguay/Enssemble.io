idf_component_register(
    SRCS "main.c" "ble_midi.c" "synth_engine.c"
    INCLUDE_DIRS "."
    REQUIRES bt nvs_flash amy
)
