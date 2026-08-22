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
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
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
    ../app/src/system_coordinator.c
)

target_include_directories(unit_tests PRIVATE
    mocks
    ../interfaces
    ../app/inc
)

target_link_libraries(unit_tests PRIVATE
    gtest_main
    motion_service
    health_monitor
    fieldbus_service
    io_service
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
