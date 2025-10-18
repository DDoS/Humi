#include "pid.h"

#include <pico/stdlib.h>

static float last_value;

void init_pid(float initial_speed)
{
    last_value = initial_speed;
}

float get_pid_output(float sensor, float target)
{
    const auto delta = target - sensor;
    last_value += delta * 0.5f;
    last_value = MIN(MAX(last_value, 0), 100);
    return last_value;
}
