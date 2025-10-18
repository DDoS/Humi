#include "climate_sensor.h"
#include "display.h"
#include "fan.h"
#include "rotary_switch.h"

#include <pico/stdio.h>
#include <pico/time.h>

#include <hardware/sync.h>

#include <stdio.h>

static volatile bool enabled = true;

static void encoder_1_click()
{
    enabled ^= true;
    __sev();
}

static volatile int8_t target_humidity = 40;
static volatile bool target_humidity_changed = true;

static void encoder_1_rotate(bool clockwise)
{
    if (!enabled) {
        return;
    }

    target_humidity = MIN(MAX(target_humidity + (clockwise ? 1 : -1), 0), 100);
    target_humidity_changed = true;
    __sev();
}

static bool sleep_until_event(uint64_t* timeout_ms)
{
    const absolute_time_t timeout_time = make_timeout_time_ms(*timeout_ms);
    bool event = false;
    do {
        if (target_humidity_changed) {
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
        encoder_1_rotate, encoder_1_click);

    bool event = false;
    uint64_t current_timeout_ms = 0;
    for (;;) {
        if (!event) {
            current_timeout_ms = 1500;
        }

        while (!enabled) {
            display_nothing();
            set_fan_speed(0);
            __wfi();
        }

        if (!event) {
            float temperature = 0;
            uint8_t humidity = 0;
            if (!read_climate(&temperature, &humidity)) {
                display_error_code(3);
            } else {
                display_number(humidity, DISPLAY_NO_DECIMAL);
                if (humidity < target_humidity) {
                    set_fan_speed(1);
                } else if (humidity >= target_humidity + 5) {
                    set_fan_speed(0);
                } else {
                    set_fan_speed((5 - (humidity - target_humidity)) / 5.f);
                }
            }
        } else if (target_humidity_changed) {
            display_number(target_humidity, 0);
            current_timeout_ms = 1000;
            target_humidity_changed = false;
        }

        // printf("%u\n", get_actual_fan_speed());

        event = sleep_until_event(&current_timeout_ms);
    }

    return 0;
}
