#include "display.h"

#include <pico/time.h>
#include <pico/critical_section.h>

#include <hardware/gpio.h>

static const uint cathode_1_pin = 2;
static const uint cathode_2_pin = 3;
static const uint anode_1_pin = 4;
static const uint anode_8_pin = 11;

static const uint8_t digit_masks[] =
{
    0b11011101,
    0b00001001,
    0b11111000,
    0b01111001,
    0b00101101,
    0b01110101,
    0b11110101,
    0b00011001,
    0b11111101,
    0b01111101,
};
static const uint8_t period_mask = 0b10;
static const uint8_t error_mask = 0b11110100;

static repeating_timer_t display_timer;
static volatile uint16_t segment_bits;

static critical_section_t critical_section;

static bool on_display_timer(__unused repeating_timer_t*)
{
    static bool odd_cycle = false;

    critical_section_enter_blocking(&critical_section);

    gpio_put(odd_cycle ? cathode_1_pin : cathode_2_pin, false);

    uint16_t segments = segment_bits;
    if (odd_cycle) {
        segments >>= 8;
    }

    for (uint i = 0, pin = anode_1_pin; pin <= anode_8_pin; i++, pin++) {
        gpio_put(pin, segments & (1 << i));
    }

    gpio_put(odd_cycle ? cathode_2_pin : cathode_1_pin, true);

    odd_cycle ^= true;

    critical_section_exit(&critical_section);

    return true;
}

bool init_display(uint refresh_ms)
{
    for (uint i = 2; i <= 11; i++) {
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
    }

    critical_section_init(&critical_section);

    return add_repeating_timer_ms(refresh_ms, on_display_timer,
        NULL, &display_timer);
}

void display_number(uint8_t number, uint8_t decimals)
{
    number = MIN(number, 100);
    uint16_t segments = digit_masks[number % 10];
    if (number >= 100) {
        segments |= digit_masks[0] << 8;
    } else if (number >= 10) {
        segments |= digit_masks[number / 10] << 8;
    }
    if (decimals & DISPLAY_DECIMAL_0) {
        segments |= period_mask;
    }
    if (decimals & DISPLAY_DECIMAL_1) {
        segments |= period_mask << 8;
    }
    segment_bits = segments;
}

void display_error_code(uint8_t code_digit)
{
    uint16_t segments = digit_masks[MIN(code_digit, 9)];
    segments |= error_mask << 8;
    segment_bits = segments;
}

void display_nothing()
{
    segment_bits = 0;
}
