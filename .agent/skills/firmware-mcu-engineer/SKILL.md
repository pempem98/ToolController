---
name: firmware-mcu-engineer
description: >-
  Expertise of a Senior/Principal MCU Embedded Firmware Engineer. Use when developing bare-metal or RTOS-based firmware, configuring MCU peripherals (UART, SPI, I2C, CAN/CAN-FD, Timers, DMA, ADC, DAC, NVIC), writing low-level drivers, memory layout & linker scripts, power optimization, interrupt handling, bootloader/OTA, or adhering to MISRA-C/C++ guidelines.
---

# Senior MCU Firmware Engineer Skill

## Core Competencies
- **MCU Architectures**: ARM Cortex-M (M0/M3/M4/M7/M33), RISC-V, ESP32, STM32, NXP, TI, Microchip.
- **Low-Level Development**: Bare-metal register manipulation, Vendor HAL/LL (STM32 HAL/LL, MCUXpresso), CMSIS Core/DSP.
- **RTOS & Concurrency**: FreeRTOS, Zephyr, ThreadX, RT-Thread (Task scheduling, mutex/semaphores, queues, event flags, stream buffers, priority inversion prevention).
- **Peripherals & Buses**: UART/USART, SPI, I2C, CAN 2.0B / CAN-FD, DMA (Double buffering, circular, cache coherency), Timers (PWM, Input Capture, Encoder mode), ADC/DAC, NVIC interrupt priorities.
- **Memory & Storage**: DTCM/ITCM, AXI SRAM, Flash memory partitioning, EEPROM emulation, Linker scripts (`.ld`, `.icf`), MPU (Memory Protection Unit), Cache management (SCB Clean/Invalidate D-Cache).
- **Safety & Reliability**: Watchdog (IWDG/WWDG), Brown-Out Reset (BOR), CRC calculation, Error-correcting code (ECC), Safe state transitions, MISRA-C:2012 compliance.

## Standard Firmware Development Workflow
1. **Hardware & Peripheral Specification**:
   - Verify clock tree settings (HSE/HSI, PLL, SysTick, Peripheral bus prescalers APB1/APB2/AHB).
   - Check GPIO alternate function mapping, pull-up/pull-down, speed configurations, and electrical constraints.
2. **Layered Architecture Implementation**:
   - **BSP/Driver Layer**: Direct register access or LL/HAL wrappers with zero unnecessary overhead.
   - **HAL/Interface Layer**: Abstract hardware behind clean interfaces (`interfaces/`) for portable, mockable code.
   - **Service/Middleware Layer**: Business logic, protocol parsers, filter algorithms (Kalman, Low-pass, PID).
   - **Application Layer**: State machine execution, user control loop, RTOS tasks.
3. **Interrupt & Concurrency Safety**:
   - Keep ISRs minimal: Set flags or defer processing to RTOS tasks/threads (Deferred Interrupt Handling).
   - Guard shared resources using critical sections, mutexes, or atomic operations (`stdatomic.h`).
   - Configure NVIC preemption priorities systematically to prevent deadlocks and priority inversions.
4. **DMA & Cache Coherency (Cortex-M7/Cache-enabled MCUs)**:
   - Align DMA buffers to cache line size (e.g. 32 bytes) with `__attribute__((aligned(32)))`.
   - Invalidate D-Cache before reading DMA RX buffers: `SCB_InvalidateDCache_by_Addr()`.
   - Clean D-Cache before starting DMA TX: `SCB_CleanDCache_by_Addr()`.
   - Or place DMA buffers in non-cacheable MPU memory regions.
5. **Code Quality & Validation**:
   - Enforce `-Wall -Wextra -Werror -pedantic`.
   - Validate static allocations (avoid dynamic `malloc`/`free` in real-time execution loops).
   - Measure execution timing using DWT Cycle Counter (`DWT->CYCCNT`) or Logic Analyzer toggle pins.

