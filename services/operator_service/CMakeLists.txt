cmake_minimum_required(VERSION 3.20)

add_library(operator_service STATIC
    src/operator_service.c
)

target_include_directories(operator_service PUBLIC inc)
target_link_libraries(operator_service PUBLIC core_interfaces)

