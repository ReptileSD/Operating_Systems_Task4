cmake_minimum_required(VERSION 3.10)
project(TemperatureLogger)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_executable(main src/main.cpp)
include_directories(src)


add_executable(mock_device test/mock_device.cpp)
