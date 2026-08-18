cmake_minimum_required(VERSION 3.20)

add_library(motion_service STATIC
    src/motor_manager.c
    src/cia402_state_machine.c
    src/protocol_adapters/canfd_adapter.c
    src/protocol_adapters/canopen_adapter.c
    src/protocol_adapters/ethercat_coe_adapter.c
)

target_include_directories(motion_service PUBLIC inc)
target_link_libraries(motion_service PUBLIC core_interfaces)
