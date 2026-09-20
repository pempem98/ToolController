cmake_minimum_required(VERSION 3.20)

add_library(middleware_freertos STATIC
    src/freertos_osal.c
)

target_include_directories(middleware_freertos PUBLIC inc)
 
if(NOT PLATFORM STREQUAL "host" AND NOT BUILD_TESTS)
    target_compile_definitions(middleware_freertos PUBLIC USE_FREERTOS)
    target_include_directories(middleware_freertos PRIVATE ${PLATFORM_INCLUDES})
endif()
