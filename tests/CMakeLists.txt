cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows MSVC runtime flexibility if needed
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
    test_motor_manager.cpp
    test_cia402_states.cpp
    test_task_watchdog.cpp
)

target_include_directories(unit_tests PRIVATE
    mocks
    ../interfaces
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
