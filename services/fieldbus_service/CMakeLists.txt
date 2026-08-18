cmake_minimum_required(VERSION 3.20)

add_library(fieldbus_service STATIC
    src/fieldbus_router.c
)

target_include_directories(fieldbus_service PUBLIC inc)
target_link_libraries(fieldbus_service PUBLIC core_interfaces)
