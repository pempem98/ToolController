cmake_minimum_required(VERSION 3.20)

add_library(cli_service STATIC
    src/cli_service.c
)

target_include_directories(cli_service PUBLIC inc)
target_link_libraries(cli_service PUBLIC
    core_interfaces
    connectivity
    motor_service
    operator_service
    brake_service
)
