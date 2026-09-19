---
name: system-architect
description: >-
  Expertise of a Principal Embedded System Architect. Use when defining MCU hardware/software partitioning, layered modular software architectures, memory map planning, RTOS task decomposition and concurrency models, inter-process communication (IPC), state machine design (Hierarchical State Machines/FSM), and real-time execution budgeting.
---

# Principal Embedded System Architect Skill

## Core Competencies
- **Hardware/Software Co-Design**: Target MCU selection, clock tree architecture, power domain budgeting, pin multiplexing, external bus matrix (FMC/OCTOSPI).
- **Layered Architecture & Modularity**: Clean Architecture for Embedded (HAL / Driver -> OSAL / Middleware -> Domain Services -> Application/Coordination).
- **Concurrency & Scheduling Models**: Preemptive RTOS, Cooperative Super-loops, Event-driven Active Object pattern, Rate Monotonic Scheduling (RMS), Worst-Case Execution Time (WCET) estimation.
- **Memory Map & Resource Partitioning**: Flash sector allocation (Bootloader, App Slot A/B, Config Data, Storage), RAM segregation (DTCM for deterministic ISRs, ITCM for critical math/algorithms, AXI SRAM for DMA buffers).
- **State Machine & Control Systems**: Finite State Machines (FSM), Hierarchical State Machines (HSM / QP-like), Event dispatching, non-blocking asynchronous state transitions.
- **IPC & Synchronization**: Message queues, lock-free ring buffers, publish-subscribe buses, zero-copy pointer exchanges with ownership semantics.

## System Architecture Workflow
1. **System Decomposition & Boundary Definition**:
   - Deconstruct system into decoupled domain services (e.g., Motion Service, Health Monitor, Fieldbus Service, IO Service).
   - Define abstract C/C++ interfaces (`interfaces/`) hiding vendor-specific hardware APIs to enable 100% Host-based Unit Testing (SIL).
2. **Task & Concurrency Design**:
   - Determine RTOS tasks based on timing criticalities and periods (e.g., 1kHz Fast Control Task, 50Hz Communication Task, 10Hz Housekeeping/Telemetry Task).
   - Assign static task stack sizes and priority levels to eliminate runtime stack collision and minimize priority inversion.
3. **Flat Interface & Hardware Isolation Principles (Clean Architecture)**:
   - **Flat `interfaces/`**: Toàn bộ interface C thuần được đặt tại gốc `interfaces/` theo mô hình phẳng, ánh xạ 1-1 trực quan với Domain Services và Hardware Drivers (không chia thư mục con gây khó tìm file).
   - **Hardware Bus Encapsulation**: Các hợp đồng giao tiếp bus vật lý (`can_interface.h`, `uart_interface.h`) phải được cô lập hoàn toàn tại `platform/common/inc/`, tuyệt đối không xuất hiện ở `interfaces/`.
   - **Stream Abstraction**: Tầng ứng dụng và CLI chỉ giao tiếp qua `console_interface.h` (pure stream I/O: `write`, `read`), không phụ thuộc UART hay USB CDC.
4. **Memory & Cache Architecture**:
   - Plan linker script layouts: Code, Read-only data, Zero-init (BSS), Non-cached DMA RAM buffers, Retentive Backup RAM.
   - Define MPU memory attributes (Normal, Device, Strongly-Ordered) to prevent unauthorized peripheral or memory access.
5. **Doxygen & Documentation Standards**:
   - Mọi hàm và struct trong `.h` và `.c` phải tuân thủ chuẩn Doxygen đầy đủ: `@brief`, `@param[in/out]`, `@return`, `@note`, `@warning`.


