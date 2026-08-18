cmake_minimum_required(VERSION 3.20)

add_executable(my_embedded_project_app
    src/main_app.c
    src/system_coordinator.c
    src/rtos_tasks.c
    
    # STM32F4 Core
    ../platform/stm32f4/cubemx/Core/Src/main.c
    ../platform/stm32f4/cubemx/Core/Src/stm32f4xx_it.c
    ../platform/stm32f4/cubemx/Core/Src/stm32f4xx_hal_msp.c
    ../platform/stm32f4/cubemx/Core/Src/system_stm32f4xx.c
    ../platform/stm32f4/cubemx/Core/Src/can.c
    ../platform/stm32f4/cubemx/Core/Src/gpio.c
    ../platform/stm32f4/cubemx/Core/Src/syscalls.c
    ../platform/stm32f4/cubemx/Core/Src/sysmem.c
    ../platform/stm32f4/cubemx/startup_stm32f407xx.s

    # STM32F4 HAL Drivers
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_can.c
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c

    # Platform driver adapters
    ../platform/stm32f4/driver_adapter/can_stm32f4_adapter.c
    ../platform/stm32f4/driver_adapter/gpio_stm32f4_adapter.c
    ../platform/stm32f4/driver_adapter/wdt_stm32f4_adapter.c
)

# Set executable extension to .elf
set_target_properties(my_embedded_project_app PROPERTIES OUTPUT_NAME "my_embedded_project_app.elf")

target_compile_definitions(my_embedded_project_app PRIVATE
    USE_HAL_DRIVER
    STM32F407xx
)

target_include_directories(my_embedded_project_app PRIVATE
    inc
    ../platform/stm32f4/bsp
    ../platform/stm32f4/cubemx/Core/Inc
    ../platform/stm32f4/cubemx/Drivers/STM32F4xx_HAL_Driver/Inc
    ../platform/stm32f4/cubemx/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ../platform/stm32f4/cubemx/Drivers/CMSIS/Include
)

target_link_options(my_embedded_project_app PRIVATE
    -T${CMAKE_CURRENT_SOURCE_DIR}/../platform/stm32f4/cubemx/STM32F407xx_FLASH.ld
)

target_link_libraries(my_embedded_project_app PRIVATE
    core_interfaces
    motion_service
    health_monitor
    fieldbus_service
    io_service
)

# Post-build: Generate .hex and .bin files automatically
add_custom_command(TARGET my_embedded_project_app POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:my_embedded_project_app> ${CMAKE_CURRENT_BINARY_DIR}/my_embedded_project_app.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary -S $<TARGET_FILE:my_embedded_project_app> ${CMAKE_CURRENT_BINARY_DIR}/my_embedded_project_app.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:my_embedded_project_app>
    COMMENT "Generating HEX and BIN binary files..."
)
