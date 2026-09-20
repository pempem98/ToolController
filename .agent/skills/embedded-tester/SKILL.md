---
name: embedded-tester
description: >-
  Expertise of a Lead Embedded QA & Test Engineer. Use when designing and writing unit tests (Unity, CMock, GoogleTest, CppUTest), Software-in-the-Loop (SIL) simulation, Hardware-in-the-Loop (HIL) automation, hardware mocking/stubbing, boundary value & fault injection testing, code coverage analysis (gcov, lcov), or automated CI/CD test pipelines.
---

# Lead Embedded QA & Test Engineer Skill

## Core Competencies
- **Embedded Unit Testing Frameworks**: Unity, CMock, GoogleTest / GoogleMock, CppUTest, Ceedling.
- **Hardware Abstraction & Mocking**: Mocking MCU HAL/LL, registers, I2C/SPI sensors, timer interrupts, and RTOS primitives to run 100% on host PC (x86_64/ARM64).
- **Test Methodologies**:
  - Boundary Value Analysis (BVA), Equivalence Partitioning, State Transition Testing.
  - Fault Injection: Memory allocation failures, corrupted sensor readings, communication timeouts, buffer overflows.
  - Stress & Long-run Testing: Heap fragmentation, ring-buffer wraparounds, integer overflows.
- **Code Coverage & Static Analysis**: Statement, Branch, MC/DC coverage via `gcc --coverage`, `gcov`, `lcov`, `llvm-cov`, static analysis with `cppcheck`, `clang-tidy`.
- **System-Level Testing (SIL / HIL)**: Host-based simulation environments (`sim` platform), automated serial/CAN stimulation, PyTest hardware automation test harnesses.

## Standard Embedded Testing Workflow
1. **Host-Based Unit Testing (SIL)**:
   - Compile target logic on Host compiler (`gcc`/`clang`) using interface mocks (e.g. `interfaces/i_gpio.h`, `interfaces/i_bus.h`).
   - Mock time and RTOS scheduling via virtual ticks or fake time providers.
2. **Writing High-Reliability Unit Tests**:
   - Structure tests with clear **Arrange - Act - Assert** (AAA).
   - Test happy paths, edge boundaries, error returns, null pointers, and buffer limit conditions.
   - Verify deterministic state reset in `setUp()` and `tearDown()`.
3. **Hardware & Peripheral Mocking with CMock/GMock**:
   - Generate mocks for all external interfaces.
   - Set strict expectations on call counts, parameter values, and return simulated error codes to verify error handling paths.
4. **Coverage & Quality Verification**:
   - Run test suite with `--coverage` enabled via preset `coverage`.
   - Generate interactive HTML coverage reports with `gcovr`:
     `gcovr --root . --filter services --filter connectivity --filter middleware --filter app --exclude tests --exclude build --html --html-details -o build/coverage_report/index.html`
   - Báo cáo HTML trực quan hỗ trợ drill-down từng dòng code, tỷ lệ Line / Function / Branch coverage.

## Gap Đã Biết (Known Gaps — Cập Nhật 2026-09-20)
- **`app/src/rtos_tasks.c` không có test thực**: `tests/test_main.c` định nghĩa `emergency_brake_task`, `motion_control_task`, `input_scan_task` thành stub rỗng để thoả linker. Toàn bộ logic task thực (bao gồm đường xử lý dừng khẩn cấp) KHÔNG nằm trong 65 test case. Con số "65/65 Passed" không phản ánh coverage của chính task orchestration layer. Ưu tiên P0: viết test gọi trực tiếp `emergency_brake_task`/`motion_control_task_step`/`input_scan_task_step`.
- **Không có static analysis / MISRA check tự động trong CI**: `coding_standards.md` yêu cầu tuân thủ MISRA-C nhưng không có `cppcheck`/`clang-tidy` chạy trong `.github/workflows/ci.yml`. Khi thêm, chạy như 1 job riêng song song với `unit-tests-and-coverage`, không chặn build nếu chưa baseline được false-positive.
- **Chưa có test cho race condition / staleness của input an toàn**: xem `.agent/rules/orchestration_protocol.md` — Verifier phải tự thêm test khi review bất kỳ thay đổi nào chạm `emergency_brake_triggered`, ADC operator input, hoặc brake path.

## Surgical Instrument Controller Test Architecture (65 Tests - 100% Passed)
- **Operator Service Tests** (`test_operator_service.cpp`): Deadband 5% filtering, joystick threshold, brake button.
- **Motor Service Tests** (`test_motor_service.cpp`): NULL guards, bind driver, multi-axis enable/disable, move_to, rotate, actual position & encoder feedback, homing and stop.
- **Brake Service Tests** (`test_brake_service.cpp`): Mechanical contact timing, state transitions, fail-safe NULL guards.
- **FreeRTOS OSAL Tests** (`test_freertos_osal.cpp`): Host POSIX/Windows emulation of tasks, queues, mutexes, and software timers.
- **System Coordinator Tests** (`test_system_coordinator.cpp`): Dependency Injection resolution, hardware binding, emergency brake propagation.


