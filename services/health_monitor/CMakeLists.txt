cmake_minimum_required(VERSION 3.20)

add_library(health_monitor STATIC
    src/watchdog_manager.c
    src/heartbeat_service.c
)

target_include_directories(health_monitor PUBLIC inc)
target_link_libraries(health_monitor PUBLIC core_interfaces)
