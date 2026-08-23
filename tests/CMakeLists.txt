cmake_minimum_required(VERSION 3.20)

option(ENABLE_COVERAGE "Enable code coverage flags" OFF)

if(ENABLE_COVERAGE)
    message(STATUS "Building with code coverage enabled")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows MSVC runtime flexibility if needed
set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
    app/test_system_coordinator.cpp
    services/motion_service/test_cia402_states.cpp
    services/motion_service/test_motor_manager.cpp
    services/health_monitor/test_heartbeat_service.cpp
    services/health_monitor/test_task_watchdog.cpp
    services/fieldbus_service/test_fieldbus_router.cpp
    services/io_service/test_brake_controller.cpp
    services/io_service/test_sensor_processor.cpp
    middleware/test_freertos_osal.cpp
    middleware/test_canopen_nmt.cpp
    middleware/test_canopen_sdo.cpp
    middleware/test_canopen_pdo.cpp
    middleware/test_canopen_od.cpp
    middleware/test_canopen_fd.cpp
    middleware/test_ethercat_al.cpp
    middleware/test_ethercat_coe.cpp
    middleware/test_ethercat_soe.cpp
    middleware/test_ethercat_eoe.cpp
    middleware/test_ethercat_slave.cpp
    services/motion_service/test_protocol_adapters.cpp
    ../app/src/system_coordinator.c
)

target_include_directories(unit_tests PRIVATE
    mocks
    ../interfaces
    ../app/inc
    ../middleware/freertos/inc
    ../middleware/canopen_stack/inc
    ../middleware/ethercat_stack/inc
)

target_link_libraries(unit_tests PRIVATE
    gtest_main
    motion_service
    health_monitor
    fieldbus_service
    io_service
    middleware_freertos
    middleware_canopen
    middleware_ethercat
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
