cmake_minimum_required(VERSION 3.20)

add_library(connectivity STATIC
    src/cli_protocol.c
)

target_include_directories(connectivity PUBLIC inc)
target_link_libraries(connectivity PUBLIC core_interfaces)
