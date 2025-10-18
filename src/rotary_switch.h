#pragma once

#include <pico/types.h>

struct rotary_switch;

typedef void (*rotate_callback)(bool clockwise);
typedef void (*click_callback)();

void init_rotary_switch(uint pin_a, uint pin_b, uint pin_click,
    rotate_callback on_rotate, click_callback on_click);
