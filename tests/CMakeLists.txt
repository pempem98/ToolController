cmake_minimum_required(VERSION 3.20)
project(unit_tests C CXX)

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

# Auto-scan all test C++ files in tests directory and subdirectories
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_executable(unit_tests
    ${TEST_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/../app/src/system_coordinator.c
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
gtest_discover_tests(unit_tests DISCOVERY_MODE PRE_TEST)
