#include "rotary_switch.h"

#include <pico/time.h>

#include <hardware/gpio.h>

static constexpr uint max_switches = 13;
struct rotary_switch
{
    uint gpio_a : 5;
    uint gpio_b : 5;
    uint gpio_click : 5;
    bool counterclockwise : 1;
    bool clockwise : 1;
    uint64_t last_input_time_us;
    rotate_callback on_rotate;
    click_callback on_click;
} switches[max_switches];
static uint next_switch_index = 0;

static struct rotary_switch * volatile switch_by_gpio[max_switches * 2 + 1];

static const uint64_t rotate_debounce_time_us = 50'000;
static const uint64_t switch_debounce_time_us = 200'000;

static void on_encoder_signal_fall(uint gpio, __unused uint32_t)
{
    struct rotary_switch * const switch_ = switch_by_gpio[gpio];
    if (!switch_) {
        return;
    }

    if (gpio == switch_->gpio_click) {
        const uint64_t now_us = to_us_since_boot(get_absolute_time());
        if (now_us - switch_->last_input_time_us < switch_debounce_time_us) {
            return;
        }
        switch_->last_input_time_us = now_us;

        if (switch_->on_click) {
            switch_->on_click();
        }
        return;
    }

    const uint64_t now_us = to_us_since_boot(get_absolute_time());
    if (now_us - switch_->last_input_time_us < rotate_debounce_time_us) {
        return;
    }

    const uint32_t gpio_state = gpio_get_all();
    const bool a_state = gpio_state & (1 << switch_->gpio_a);
    const bool b_state = gpio_state & (1 << switch_->gpio_b);

    if (gpio == switch_->gpio_a) {
        if (b_state && !a_state) {
            switch_->clockwise = true;
        }
        if (switch_->counterclockwise && !b_state && !a_state) {
            switch_->clockwise = false;
            switch_->counterclockwise = false;
            switch_->last_input_time_us = now_us;
            if (switch_->on_rotate) {
                switch_->on_rotate(false);
            }
        }
    }
    if (gpio == switch_->gpio_b) {
        if (!b_state && a_state) {
            switch_->counterclockwise = true;
        }
        if (switch_->clockwise && !a_state && !b_state) {
            switch_->clockwise = false;
            switch_->counterclockwise = false;
            switch_->last_input_time_us = now_us;
            if (switch_->on_rotate) {
                switch_->on_rotate(true);
            }
        }
    }
}

void init_rotary_switch(uint gpio_a, uint gpio_b, uint gpio_click,
    rotate_callback on_rotate, click_callback on_click)
{
    gpio_init(gpio_a);
    gpio_disable_pulls(gpio_a);
    gpio_init(gpio_b);
    gpio_disable_pulls(gpio_b);
    gpio_init(gpio_click);
    gpio_disable_pulls(gpio_click);

    gpio_set_irq_enabled_with_callback(gpio_a,
        GPIO_IRQ_EDGE_FALL, true,
        on_encoder_signal_fall);
    gpio_set_irq_enabled(gpio_b, GPIO_IRQ_EDGE_FALL, true);
    if (gpio_click) {
        gpio_set_irq_enabled(gpio_click, GPIO_IRQ_EDGE_FALL, true);
    }

    struct rotary_switch *switch_ = &switches[next_switch_index++];
    switch_->gpio_a = gpio_a;
    switch_->gpio_b = gpio_b;
    switch_->gpio_click = gpio_click;
    switch_->last_input_time_us = to_us_since_boot(get_absolute_time());
    switch_->on_rotate = on_rotate;
    switch_->on_click = on_click;
    switch_by_gpio[gpio_a] = switch_;
    switch_by_gpio[gpio_b] = switch_;
    switch_by_gpio[gpio_click] = switch_;
}
