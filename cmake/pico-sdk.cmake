include_guard()

include(FetchContent)
FetchContent_Declare(pico-sdk
    GIT_REPOSITORY https://github.com/raspberrypi/pico-sdk.git
    GIT_TAG 2.2.0
    GIT_SHALLOW
    GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
)
FetchContent_Populate(pico-sdk)

include("${pico-sdk_SOURCE_DIR}/pico_sdk_init.cmake")
