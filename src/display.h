#pragma once

#include <pico/types.h>

#define DISPLAY_NO_DECIMAL 0
#define DISPLAY_DECIMAL_0 0b10
#define DISPLAY_DECIMAL_1 0b1

bool init_display(uint refresh_ms);

void display_number(uint8_t number, uint8_t decimals);

void display_error_code(uint8_t code_digit);

void display_nothing();
