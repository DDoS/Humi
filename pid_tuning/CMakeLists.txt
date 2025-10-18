cmake_minimum_required(VERSION 3.23)
cmake_policy(SET CMP0169 OLD)

project(pid_tuning LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

include("../cmake/pybind11.cmake")

find_package(Python 3 COMPONENTS Interpreter Development REQUIRED)

pybind11_add_module(pid_tuning "pid_tuning.cpp")
