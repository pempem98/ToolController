cmake_minimum_required(VERSION 3.20)

add_library(io_service STATIC
    src/brake_controller.c
    src/sensor_processor.c
)

target_include_directories(io_service PUBLIC inc)
target_link_libraries(io_service PUBLIC core_interfaces)
