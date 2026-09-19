cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED PLATFORM OR PLATFORM MATCHES "^\\$\\{" OR PLATFORM STREQUAL "")
    set(PLATFORM "stm32h7a3zit6q")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/../platform/${PLATFORM}/platform.cmake")
    set(PLATFORM "stm32h7a3zit6q")
endif()


# Include target platform specification
include("${CMAKE_CURRENT_LIST_DIR}/../platform/${PLATFORM}/platform.cmake")


set(APP_NAME "surgical_instrument_controller_${PLATFORM}")

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
    connectivity
    motor_service
    brake_service
    operator_service
    cli_service
    middleware_freertos
)

# Post-build: Generate .hex and .bin files automatically for hardware targets
if(NOT PLATFORM STREQUAL "host")
    get_filename_component(TOOLCHAIN_BIN_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
    find_program(OBJCOPY_BIN arm-none-eabi-objcopy HINTS "${TOOLCHAIN_BIN_DIR}")
    find_program(SIZE_BIN arm-none-eabi-size HINTS "${TOOLCHAIN_BIN_DIR}")

    if(NOT OBJCOPY_BIN)
        set(OBJCOPY_BIN arm-none-eabi-objcopy)
    endif()
    if(NOT SIZE_BIN)
        set(SIZE_BIN arm-none-eabi-size)
    endif()

    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${OBJCOPY_BIN} -O ihex $<TARGET_FILE:${APP_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.hex
        COMMAND ${OBJCOPY_BIN} -O binary -S $<TARGET_FILE:${APP_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.bin
        COMMAND ${SIZE_BIN} $<TARGET_FILE:${APP_NAME}>
        COMMENT "Generating HEX and BIN binary files for ${APP_NAME}..."
    )
endif()

