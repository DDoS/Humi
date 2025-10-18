#include "climate_sensor.h"
#include "display.h"
#include "fan.h"
#include "pid.h"
#include "rotary_switch.h"

#include <pico/stdio.h>
#include <pico/time.h>

#include <hardware/sync.h>

#include <math.h>

static volatile bool enabled = true;

static void encoder_2_click()
{
    enabled ^= true;
    __sev();
}

static volatile int8_t target_humidity = 40;
static volatile bool target_humidity_changed = false;

static void encoder_1_rotate(bool clockwise)
{
    if (!enabled) {
        return;
    }

    target_humidity = MIN(MAX(target_humidity + (clockwise ? 1 : -1), 0), 100);
    target_humidity_changed = true;
    __sev();
}

static volatile uint8_t display_index = 0;
static volatile bool display_index_changed = false;

static void encoder_1_click()
{
    display_index = 4;
    display_index_changed = true;
    __sev();
}

static void encoder_2_rotate(bool clockwise)
{
    if (!enabled) {
        return;
    }

    display_index = (display_index + (clockwise ? 1 : -1)) & 0b11;
    display_index_changed = true;
    __sev();
}

static bool sleep_until_event(uint64_t* timeout_ms)
{
    const absolute_time_t timeout_time = make_timeout_time_ms(*timeout_ms);
    bool event = false;
    do {
        if (target_humidity_changed || display_index_changed) {
            event = true;
            break;
        }
    } while (!best_effort_wfe_or_timeout(timeout_time));

    if (event) {
        const uint64_t current_time_ms = to_ms_since_boot(get_absolute_time());
        const uint64_t start_time_ms = to_ms_since_boot(timeout_time);
        if (start_time_ms >= current_time_ms) {
            *timeout_ms = start_time_ms - current_time_ms;
        } else {
            *timeout_ms = 0;
        }
    }

    return event;
}

int main()
{
    stdio_init_all();

    if (!init_display(5)) {
        return 1;
    }

    if (!init_climate_sensor()) {
        display_error_code(1);
    }

    if (!init_fan(2'500)) {
        display_error_code(2);
    }

    init_rotary_switch(15, 14, 13,
        encoder_1_rotate, encoder_1_click);

    init_rotary_switch(22, 21, 18,
        encoder_2_rotate, encoder_2_click);

    init_pid(50);

    float current_temperature = 0;
    uint8_t current_humidity = 0;
    float fan_speed = 0;

    bool event = false;
    uint64_t current_timeout_ms = 0;
    for (;;) {
        while (!enabled) {
            display_nothing();
            set_fan_speed(0);
            __wfi();
        }

        if (!event) {
            if (!read_climate(&current_temperature, &current_humidity)) {
                display_error_code(3);
            } else {
                display_number(current_humidity, DISPLAY_NO_DECIMAL);
                fan_speed = get_pid_output(current_humidity, target_humidity);
                set_fan_speed(fan_speed);
            }
            display_index = 0;
            current_timeout_ms = 1500;
        } else if (target_humidity_changed) {
            display_number(target_humidity, DISPLAY_DECIMAL_0);
            current_timeout_ms = 1000;
            target_humidity_changed = false;
        } else if (display_index_changed) {
            switch (display_index) {
            case 0:
                display_number(current_humidity, DISPLAY_NO_DECIMAL);
                break;
            case 1:
                display_number(target_humidity, DISPLAY_DECIMAL_0);
                break;
            case 2:
                display_number((uint8_t)lround(fan_speed), DISPLAY_DECIMAL_1);
                break;
            case 3:
                display_number((uint8_t)lround(get_actual_fan_speed() * 0.1f),
                    DISPLAY_DECIMAL_0 | DISPLAY_DECIMAL_1);
                break;
            case 4:
                display_number((uint8_t)lround(current_temperature), DISPLAY_NO_DECIMAL);
                break;
            }
            current_timeout_ms = 2000;
            display_index_changed = false;
        }

        event = sleep_until_event(&current_timeout_ms);
    }

    return 0;
}
