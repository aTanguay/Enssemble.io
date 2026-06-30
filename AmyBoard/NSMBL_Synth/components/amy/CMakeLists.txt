idf_component_register(
    SRCS
        "algorithms.c"
        "amy.c"
        "amy-message.c"
        "amy-piano.c"
        "amy_midi.c"
        "api.c"
        "custom.c"
        "cv_trigger.c"
        "delay.c"
        "envelope.c"
        "examples.c"
        "filters.c"
        "i2s.c"
        "instrument.c"
        "interp_partials.c"
        "log2_exp2.c"
        "midi_mappings.c"
        "oscillators.c"
        "parse.c"
        "patches.c"
        "pcm.c"
        "sequencer.c"
        "transfer.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_driver_i2s esp_driver_spi esp_timer esp_driver_i2c
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC
    ESP_PLATFORM
)
