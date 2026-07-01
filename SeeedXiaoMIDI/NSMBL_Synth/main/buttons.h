#pragma once

#include <stdint.h>

typedef void (*button_cb_t)(uint8_t index);

void buttons_init(button_cb_t on_short, button_cb_t on_long);
void buttons_poll(void);
