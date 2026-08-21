set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Specify compilers
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(CMAKE_SIZE arm-none-eabi-size)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Target specs & flags with Debug Symbols (-O0 -g3)
set(CPU_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb")
set(CMAKE_C_FLAGS "${CPU_FLAGS} -O0 -g3 -fdata-sections -ffunction-sections -Wall -Wextra" CACHE STRING "C Flags")
set(CMAKE_CXX_FLAGS "${CPU_FLAGS} -O0 -g3 -fdata-sections -ffunction-sections -Wall -Wextra" CACHE STRING "CXX Flags")
set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} --specs=nano.specs -Wl,--gc-sections" CACHE STRING "Linker Flags")
