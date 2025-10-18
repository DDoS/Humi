include_guard()

include(FetchContent)

FetchContent_Declare(pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG stable
    GIT_SHALLOW
    GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
)
FetchContent_MakeAvailable(pybind11)
