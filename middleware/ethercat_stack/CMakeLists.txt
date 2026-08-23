cmake_minimum_required(VERSION 3.20)

add_library(middleware_ethercat STATIC
    src/ethercat_al.c
    src/ethercat_coe.c
    src/ethercat_soe.c
    src/ethercat_eoe.c
    src/ethercat_slave.c
)

target_include_directories(middleware_ethercat PUBLIC inc)
target_link_libraries(middleware_ethercat PUBLIC core_interfaces middleware_canopen)
