include_guard()

include(FetchContent)
FetchContent_Declare(aht30
    GIT_REPOSITORY https://github.com/libdriver/aht30.git
    GIT_TAG v1.0.5
    GIT_SHALLOW
    GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
    PATCH_COMMAND ${CMAKE_COMMAND}
        -Din:FILEPATH=src/driver_aht30.c
        -Dout:FILEPATH=src/driver_aht30.c
        -Dpatch:FILEPATH=${PROJECT_SOURCE_DIR}/external/driver_aht30.c.patch
        -P ${PROJECT_SOURCE_DIR}/cmake/PatchFile.cmake
)
FetchContent_Populate(aht30)
add_library(aht30 STATIC)
target_sources(aht30 PRIVATE
    "${aht30_SOURCE_DIR}/src/driver_aht30.c"
    "${aht30_SOURCE_DIR}/src/driver_aht30.h")
target_include_directories(aht30 PUBLIC
    "${aht30_SOURCE_DIR}/src"
    "${aht30_SOURCE_DIR}/interface")
