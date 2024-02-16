cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(read_temprature C CXX ASM)

set(CMAKE_C_STANDARD 11)

set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(read_temprature
read_temprature.cpp
)

# pull in common dependencies
target_link_libraries(read_temprature pico_stdlib hardware_uart hardware_i2c)
 
# create map/bin/hex file etc.
pico_add_extra_outputs(read_temprature)

pico_enable_stdio_uart(read_temprature 1)