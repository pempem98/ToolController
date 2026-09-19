cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED PLATFORM OR PLATFORM MATCHES "^\\$\\{" OR PLATFORM STREQUAL "")
    set(PLATFORM "stm32h7a3ziq" CACHE STRING "Target hardware platform (stm32h7a3ziq, host)" FORCE)
endif()

if(NOT CMAKE_TOOLCHAIN_FILE AND NOT PLATFORM STREQUAL "host" AND NOT BUILD_TESTS)
    include("${CMAKE_CURRENT_LIST_DIR}/arm-none-eabi-gcc.cmake")
endif()

if(NOT PLATFORM STREQUAL "host" AND NOT BUILD_TESTS)
    project(my_embedded_project C CXX ASM)
else()
    project(my_embedded_project C CXX)
endif()



set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS "Build unit tests for host PC" OFF)
option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

if(ENABLE_COVERAGE AND (PLATFORM STREQUAL "host" OR BUILD_TESTS))
    message(STATUS "Code coverage instrumentation ENABLED")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

# Include interfaces
add_subdirectory(interfaces)

# Include connectivity (protocol codecs)
add_subdirectory(connectivity)

# Include middleware
add_subdirectory(middleware)

# Include business logic services
add_subdirectory(services/motor_service)
add_subdirectory(services/brake_service)
add_subdirectory(services/operator_service)
add_subdirectory(services/cli_service)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
else()
    # Main embedded binary application
    add_subdirectory(app)
endif()
