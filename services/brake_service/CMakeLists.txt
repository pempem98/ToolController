cmake_minimum_required(VERSION 3.20)

add_library(brake_service STATIC
    src/brake_service.c
)

target_include_directories(brake_service PUBLIC inc)
target_link_libraries(brake_service PUBLIC core_interfaces)

