# Simulation Platform Build Specification (Host PC / Docker)

set(PLATFORM_DEFINES
    PLATFORM_SIM
)

set(PLATFORM_INCLUDES
    ${CMAKE_CURRENT_LIST_DIR}/bsp
)

file(GLOB PLATFORM_ADAPTER_SOURCES "${CMAKE_CURRENT_LIST_DIR}/driver_adapter/*.c")

set(PLATFORM_SOURCES
    ${PLATFORM_ADAPTER_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/sim_main.c
    ${CMAKE_CURRENT_LIST_DIR}/sim_syscalls.c
)

set(PLATFORM_LINK_OPTIONS "")
