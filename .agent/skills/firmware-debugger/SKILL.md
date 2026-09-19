---
name: firmware-debugger
description: >-
  Expertise of a Senior Embedded Systems & Firmware Debugger. Use when diagnosing ARM Cortex-M HardFault / BusFault / UsageFault / MemManage exceptions, stack overflows, heap corruption, memory leaks, DMA coherency bugs, race conditions, RTOS priority inversions, watchdog resets, or analyzing dumps with GDB, OpenOCD, J-Link, logic analyzer & oscilloscope captures.
---

# Senior Embedded Firmware Debugger Skill

## Core Competencies
- **Cortex-M Exception & Fault Triage**:
  - Decode Configurable Fault Status Register (`CFSR`), HardFault Status Register (`HFSR`), BusFault Address Register (`BFAR`), MemManage Fault Address Register (`MMFAR`).
  - Extract stacked exception frame: `R0-R3`, `R12`, `LR` (EXC_RETURN), `PC` (Faulting instruction address), `xPSR`.
  - Distinguish precise BusFaults (invalid memory read/write) vs imprecise BusFaults (delayed write buffer error; disable buffered writes via `ACTLR.DISDEFWBUF` to localize).
- **Memory & Stack Diagnostics**:
  - Stack overflow detection: Stack watermarking (Fill stack with `0xA5A5A5A5`), MPU stack guard zones.
  - Heap corruption & leaks: Static allocation auditing, double-free detection, buffer overruns.
- **Hardware-Assisted Debugging Tools**:
  - GDB CLI / Embedded GDB, OpenOCD, SEGGER J-Link (RTT, SystemView for RTOS execution tracing), ST-Link.
  - ITM (Instrumentation Trace Macrocell) printf with zero CPU blocking overhead.
  - Logic Analyzer / Oscilloscope decoding (SPI, I2C, UART, CAN, timing jitter, glitch capture).
- **Concurrency & Timing Bug Identification**:
  - Priority inversion, deadlocks, RTOS task starvation, race conditions on non-atomic read-modify-write operations, ISR re-entrancy, missing `volatile` qualifiers on memory-mapped registers / shared flags.
  - DMA / CPU Cache Incoherency bugs on Cortex-M7 (stale D-Cache reads or overwriting DMA buffers).

## Standard Firmware Debugging Workflow
1. **Fault Trap & Register Extraction**:
   - Implement custom assembly `HardFault_Handler` to extract Main Stack Pointer (`MSP`) or Process Stack Pointer (`PSP`).
   - Read faulting `PC` and locate line of code in `.elf` or `.map` file:
     ```bash
     arm-none-eabi-addr2line -e build/ToolController.elf -f -C <PC_ADDRESS>
     arm-none-eabi-gdb build/ToolController.elf -ex "info line *<PC_ADDRESS>" -batch
     ```
2. **Crash Dump & Call Stack Reconstruction**:
   - Inspect Link Register `LR` and stack frame memory backtrace to reconstruct caller execution path.
   - Check if fault was caused by:
     - Null pointer dereference (`MMFAR` = 0x00000000).
     - Unaligned memory access (`CFSR.UNALIGNED`).
     - Undefined instruction execution (`CFSR.UNDEFINSTR`).
     - Branch to invalid / non-Thumb address (LSB of target address not set to 1).
3. **Deadlock & RTOS Timing Investigation**:
   - Check task states (`vTaskList` / FreeRTOS Kernel Awareness in GDB).
   - Verify mutex ownership and task priorities.
   - Verify interrupt priority levels vs `configMAX_SYSCALL_INTERRUPT_PRIORITY`.
4. **Watchdog (IWDG/WWDG) & Brown-out Root Cause Analysis**:
   - Check Reset Status Register (`RCC->CSR` or `RCC->RSR`) at startup to determine cause of reset (IWDG, WWDG, Low-Power, BOR, Pin Reset).
   - Save diagnostic crash record in Retentive Backup SRAM before system reset.

