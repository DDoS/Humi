#pragma once

#include <stdint.h>

bool init_climate_sensor();

bool read_climate(float *temperature, uint8_t *humidity);
