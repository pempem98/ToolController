cmake_minimum_required(VERSION 3.20)

add_library(middleware_freertos STATIC
    src/freertos_osal.c
)

target_include_directories(middleware_freertos PUBLIC inc)
