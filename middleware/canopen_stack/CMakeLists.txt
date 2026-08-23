cmake_minimum_required(VERSION 3.20)

add_library(middleware_canopen STATIC
    src/canopen_nmt.c
    src/canopen_sdo.c
    src/canopen_pdo.c
    src/canopen_od.c
    src/canopen_node.c
)

target_include_directories(middleware_canopen PUBLIC inc)
target_link_libraries(middleware_canopen PUBLIC core_interfaces)
