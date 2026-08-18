cmake_minimum_required(VERSION 3.20)
project(my_embedded_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS "Build unit tests for host PC" OFF)

# Include interfaces
add_subdirectory(interfaces)

# Include business logic services
add_subdirectory(services/motion_service)
add_subdirectory(services/health_monitor)
add_subdirectory(services/fieldbus_service)
add_subdirectory(services/io_service)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
else()
    # Main embedded binary application
    add_subdirectory(app)
endif()
