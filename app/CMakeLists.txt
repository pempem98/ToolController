cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED PLATFORM)
    set(PLATFORM "stm32f407ve")
endif()

# Include target platform specification
include(../platform/${PLATFORM}/platform.cmake)

set(APP_NAME "tool_controller_${PLATFORM}")

add_executable(${APP_NAME}
    src/main_app.c
    src/system_coordinator.c
    src/rtos_tasks.c
    ${PLATFORM_SOURCES}
)

# Set executable output filename
set_target_properties(${APP_NAME} PROPERTIES OUTPUT_NAME "${APP_NAME}.elf")

target_compile_definitions(${APP_NAME} PRIVATE ${PLATFORM_DEFINES})
target_include_directories(${APP_NAME} PRIVATE inc ${PLATFORM_INCLUDES})
target_link_options(${APP_NAME} PRIVATE ${PLATFORM_LINK_OPTIONS})

target_link_libraries(${APP_NAME} PRIVATE
    core_interfaces
    motion_service
    health_monitor
    fieldbus_service
    io_service
)

# Post-build: Generate .hex and .bin files automatically
add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${APP_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary -S $<TARGET_FILE:${APP_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${APP_NAME}>
    COMMENT "Generating HEX and BIN binary files for ${APP_NAME}..."
)
