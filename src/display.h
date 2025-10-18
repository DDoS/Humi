#pragma once

#include <pico/types.h>

#define DISPLAY_NO_DECIMAL -1

bool init_display(uint refresh_ms);

void display_number(uint8_t number, uint8_t decimal_place);

void display_error_code(uint8_t code_digit);

void display_nothing();
