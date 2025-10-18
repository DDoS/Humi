#pragma once

void init_pid(float initial_speed);

float get_pid_output(float sensor, float target);
