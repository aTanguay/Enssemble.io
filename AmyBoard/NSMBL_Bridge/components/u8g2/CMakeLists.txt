# Vendored u8g2 (olikraus/u8g2, BSD-2). csrc/u8g2_fonts.c trimmed to the fonts
# referenced by main/oled.c — restore the full file from upstream to add more.
file(GLOB U8G2_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/csrc/*.c)

idf_component_register(
    SRCS ${U8G2_SRCS}
    INCLUDE_DIRS "csrc"
)
