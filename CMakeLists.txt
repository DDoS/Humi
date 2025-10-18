cmake_minimum_required(VERSION 3.23)
cmake_policy(SET CMP0169 OLD)

include("cmake/pico-sdk.cmake")

project(humi LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

pico_sdk_init()

include("cmake/aht30.cmake")

add_executable(humi)
target_sources(humi PRIVATE
    "src/driver_aht30_interface.c"
    "src/climate_sensor.c"
    "src/climate_sensor.c"
    "src/display.h"
    "src/display.c"
    "src/fan.h"
    "src/fan.c"
    "src/main.c"
    "src/pid.c"
    "src/pid.h"
    "src/rotary_switch.h"
    "src/rotary_switch.c")
target_link_libraries(humi PRIVATE
    pico_stdlib
    pico_stdio
    pico_sync
    pico_time
    hardware_clocks
    hardware_i2c
    hardware_pwm
    hardware_sync
    aht30)
target_compile_definitions(humi PRIVATE
    PICO_DEFAULT_I2C=0
    PICO_DEFAULT_I2C_SDA_PIN=16
    PICO_DEFAULT_I2C_SCL_PIN=17)

pico_enable_stdio_usb(humi ON)
pico_enable_stdio_uart(humi ON)

pico_add_extra_outputs(humi)

pico_set_program_description(humi "Humi")
pico_set_program_url(humi "https://github.com/DDoS/Humi")

include("cmake/PicoCommands.cmake")
picoprobe_add_flash_target(humi)
picoprobe_add_reset_target(humi)
picotool_add_load_target(humi)
