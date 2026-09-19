cmake_minimum_required(VERSION 3.20)

add_library(motor_service STATIC
    src/motor_service.c
)

target_include_directories(motor_service PUBLIC inc)
target_link_libraries(motor_service PUBLIC core_interfaces)

