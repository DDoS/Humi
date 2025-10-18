#include "fan.h"

#include <pico/time.h>

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>

#include <limits.h>
#include <math.h>

static const uint output_pin = 20;
static const uint input_pin = 19;

static const uint32_t pwm_frequency = 25'000;
static uint32_t pwm_wrap;

static repeating_timer_t tachometer_timer;
static volatile uint32_t tachometer_pulses;

static const float max_fan_speed_percent = 1;
static const float min_fan_speed_percent = 0.1f;

static uint32_t set_pwm_frequency(pwm_config *config, uint32_t frequency)
{
    const uint32_t clock = clock_get_hz(clk_sys);

    static const uint32_t max_wrap = USHRT_MAX + 1;
    static const uint32_t divider_base = 16;
    static const uint32_t frequency_multiplier = max_wrap / divider_base;
    const float divider = ceilf((float)clock / (frequency * frequency_multiplier)) / divider_base;
    pwm_config_set_clkdiv(config, divider);

    const uint32_t wrap = lroundf(clock / divider / frequency);
    pwm_config_set_wrap(config, wrap - 1);
    return wrap;
}

static bool on_fan_speed_timer(__unused repeating_timer_t*)
{
    const uint input_slice = pwm_gpio_to_slice_num(input_pin);
    tachometer_pulses = pwm_get_counter(input_slice);
    pwm_set_counter(input_slice, 0);
    return true;
}

bool init_fan(uint speed_readout_period_ms)
{
    const uint output_slice = pwm_gpio_to_slice_num(output_pin);
    pwm_config output_config = pwm_get_default_config();
    pwm_wrap = set_pwm_frequency(&output_config, pwm_frequency);
    pwm_init(output_slice, &output_config, true);
    gpio_set_function(output_pin, GPIO_FUNC_PWM);

    assert(pwm_gpio_to_channel(input_pin) == PWM_CHAN_B);
    const uint input_slice = pwm_gpio_to_slice_num(input_pin);
    pwm_config input_config = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&input_config, PWM_DIV_B_RISING);
    pwm_init(input_slice, &input_config, true);
    gpio_set_function(input_pin, GPIO_FUNC_PWM);

    return add_repeating_timer_ms(speed_readout_period_ms, on_fan_speed_timer,
        NULL, &tachometer_timer);
}

void set_fan_speed(float percent)
{
    if (percent < 1) {
        percent = 0;
    } else {
        percent = MAX(MIN(percent / 100, max_fan_speed_percent), min_fan_speed_percent);
    }
    pwm_set_gpio_level(output_pin, (uint16_t)lroundf(pwm_wrap * percent));
}

uint get_actual_fan_speed()
{
    return lroundf(tachometer_pulses / 2.f / (tachometer_timer.delay_us / 1e6f) * 60);
}
