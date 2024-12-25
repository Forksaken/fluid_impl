cmake_minimum_required(VERSION 3.10)
project(FluidSimulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

add_executable(fluid_simulator main.cpp)