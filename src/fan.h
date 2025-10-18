#pragma once

#include <pico/types.h>

bool init_fan(uint speed_readout_period_ms);

void set_fan_speed(float percent);

uint get_actual_fan_speed();
