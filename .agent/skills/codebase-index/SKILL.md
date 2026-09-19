---
name: codebase-index
description: Comprehensive memory index and navigational map of all files, directory structures, architectural layers, and toolchain paths in the ToolController project. Activate this skill to immediately locate any file, contract, driver, service, or toolchain binary without scanning the disk or running filesystem searches.
---

# Codebase Index & Architectural Memory (ToolController)

Tài liệu chỉ mục bộ nhớ chuyên dụng dành riêng cho AI Agent. Mục đích: **Định vị tức thì 100% file mã nguồn, thư viện và toolchain trên máy tính mà không tốn token hay thời gian quét ổ đĩa (Zero Disk Scanning overhead)**.

---

## 1. Bản Đồ Môi Trường & Toolchains (Resolved Paths on Windows)

Mọi công cụ biên dịch, liên kết và kiểm thử trên máy tính của **Kỹ sư V** được phân giải chính xác qua biến môi trường chuẩn:

| Công cụ | Phiên bản / Chi tiết | Đường dẫn tương đối chuẩn |
| :--- | :--- | :--- |
| **ARM GCC (C Compiler)** | GNU Tools for STM32 14.3.1 (ARM Cortex-M7) | `$env:LOCALAPPDATA\stm32cube\bundles\gnu-tools-for-stm32\14.3.1+st.2\bin\arm-none-eabi-gcc.exe` |
| **ARM G++ (C++ Compiler)** | GNU Tools for STM32 14.3.1 | `$env:LOCALAPPDATA\stm32cube\bundles\gnu-tools-for-stm32\14.3.1+st.2\bin\arm-none-eabi-g++.exe` |
| **ARM Objcopy & Size** | Binary utilities cho STM32 | `$env:LOCALAPPDATA\stm32cube\bundles\gnu-tools-for-stm32\14.3.1+st.2\bin\arm-none-eabi-objcopy.exe`<br/>`$env:LOCALAPPDATA\stm32cube\bundles\gnu-tools-for-stm32\14.3.1+st.2\bin\arm-none-eabi-size.exe` |
| **Host GCC/G++** | MinGW-W64 16.1.0 UCRT POSIX (WinLibs / WinGet) | `$env:LOCALAPPDATA\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\gcc.exe`<br/>`$env:LOCALAPPDATA\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\g++.exe` |
| **Coverage Tool** | `gcovr` 8.6 (Python 3.13) | `$env:LOCALAPPDATA\Programs\Python\Python313\Scripts\gcovr.exe` |
| **CMake & Ninja** | ST Bundles / Kitware | `$env:LOCALAPPDATA\stm32cube\bundles\cmake\4.3.1+st.1\bin\cmake.exe`<br/>`$env:LOCALAPPDATA\stm32cube\bundles\ninja\1.13.2+st.1\bin\ninja.exe` |
| **ST-LINK GDB Server** | STM32CubeIDE 2.2.0 | `${env:SystemDrive}/ST/STM32CubeIDE_2.2.0/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.stlink-gdb-server.win32_2.2.500.202604010938/tools/bin/ST-LINK_gdbserver.exe` |
| **STM32 Programmer** | STM32CubeProgrammer CLI | `${env:SystemDrive}/ST/STM32CubeIDE_2.2.0/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.2.500.202603051304/tools/bin` |

---

## 2. Bản Đồ Mã Nguồn Dự Án (100% File Inventory by Architecture Layer)

### 2.1. Tầng 1: Application Layer (`app/`) - Đã hoàn thiện 100% chuẩn Doxygen
- [`app/CMakeLists.txt`](app/CMakeLists.txt): Build executable `tool_controller_stm32h7a3ziq.elf`, `.hex`, `.bin`.
- [`app/inc/app_config.h`](app/inc/app_config.h): Cấu hình chu kỳ, timeout và thông số toàn cục hệ thống.
- [`app/inc/main_app.h`](app/inc/main_app.h) & [`app/src/main_app.c`](app/src/main_app.c): Điểm vào ứng dụng (`App_Init`, `App_Start`, `App_Main`).
- [`app/inc/system_coordinator.h`](app/inc/system_coordinator.h) & [`app/src/system_coordinator.c`](app/src/system_coordinator.c): Bộ điều phối trung tâm (Dependency Injection, bind driver, quản lý trạng thái, kích hoạt phanh khẩn cấp).
- [`app/inc/rtos_tasks_config.h`](app/inc/rtos_tasks_config.h): Định nghĩa Task Priority, Stack Size và Timeout của FreeRTOS.
- [`app/src/rtos_tasks.c`](app/src/rtos_tasks.c): Hiện thực 4 Task FreeRTOS cốt lõi:
  1. `EmergencyBrakeTask` (Ưu tiên Realtime/High, phản ứng ngắt < 1ms).
  2. `MotionControlTask` (Ưu tiên High, chu kỳ 10ms đồng bộ 4 trục).
  3. `InputScanTask` (Ưu tiên Normal, chu kỳ 10ms quét chiết áp & nút).
  4. `ConsoleTask` (Ưu tiên Low, chu kỳ 50ms phục vụ CLI & Telemetry).

### 2.2. Tầng 2: Domain Services Layer (`services/`) - 100% C thuần, độc lập phần cứng
- **`motor_service`**:
  - [`services/motor_service/CMakeLists.txt`](services/motor_service/CMakeLists.txt)
  - [`services/motor_service/inc/motor_service.h`](services/motor_service/inc/motor_service.h)
  - [`services/motor_service/src/motor_service.c`](services/motor_service/src/motor_service.c)
- **`brake_service`**:
  - [`services/brake_service/CMakeLists.txt`](services/brake_service/CMakeLists.txt)
  - [`services/brake_service/inc/brake_service.h`](services/brake_service/inc/brake_service.h)
  - [`services/brake_service/src/brake_service.c`](services/brake_service/src/brake_service.c)
- **`operator_service`**:
  - [`services/operator_service/CMakeLists.txt`](services/operator_service/CMakeLists.txt)
  - [`services/operator_service/inc/operator_service.h`](services/operator_service/inc/operator_service.h)
  - [`services/operator_service/src/operator_service.c`](services/operator_service/src/operator_service.c)
- **`cli_service`**:
  - [`services/cli_service/CMakeLists.txt`](services/cli_service/CMakeLists.txt)
  - [`services/cli_service/inc/cli_service.h`](services/cli_service/inc/cli_service.h)
  - [`services/cli_service/src/cli_service.c`](services/cli_service/src/cli_service.c)

### 2.3. Tầng 3: Connectivity Layer (`connectivity/`) - Giao thức & giải mã
- [`connectivity/CMakeLists.txt`](connectivity/CMakeLists.txt)
- [`connectivity/inc/cli_protocol.h`](connectivity/inc/cli_protocol.h) & [`connectivity/src/cli_protocol.c`](connectivity/src/cli_protocol.c): Parser tập lệnh text ASCII (`SET_THROTTLE`, `EMERGENCY_STOP`, `STATUS`) và định dạng telemetry.

### 2.4. Tầng 4: Pure Abstract Interfaces Layer (`interfaces/`) - Phẳng, Ánh xạ 1-1
- [`interfaces/CMakeLists.txt`](interfaces/CMakeLists.txt): Interface library `core_interfaces`.
- [`interfaces/status_types.h`](interfaces/status_types.h): Kiểu trả về chuẩn `status_t` (`STATUS_OK`, `STATUS_ERROR`, `STATUS_TIMEOUT`,...).
- [`interfaces/os_interface.h`](interfaces/os_interface.h): Hợp đồng trừu tượng hóa hệ điều hành.
- [`interfaces/motor_interface.h`](interfaces/motor_interface.h): Hợp đồng điều khiển động cơ (`init`, `set_enabled`, `set_target_velocity`, `move_to`, `get_actual_position`,...).
- [`interfaces/brake_interface.h`](interfaces/brake_interface.h): Hợp đồng điều khiển phanh (`init`, `engage`, `release`, `is_engaged`).
- [`interfaces/operator_input_interface.h`](interfaces/operator_input_interface.h): Hợp đồng nhập liệu (`read_axis`, `read_button`).
- [`interfaces/console_interface.h`](interfaces/console_interface.h): Hợp đồng Stream I/O thuần túy (`write`, `read`) cho CLI/Terminal.
- [`interfaces/board_interface.h`](interfaces/board_interface.h): Cấu trúc phần cứng board gom tụ các driver (`board_hardware_t`, `board_init`, `board_get_hardware`).

### 2.5. Tầng 5: Middleware Layer (`middleware/`)
- [`middleware/CMakeLists.txt`](middleware/CMakeLists.txt)
- [`middleware/freertos/CMakeLists.txt`](middleware/freertos/CMakeLists.txt)
- [`middleware/freertos/inc/freertos_osal.h`](middleware/freertos/inc/freertos_osal.h) & [`middleware/freertos/src/freertos_osal.c`](middleware/freertos/src/freertos_osal.c): Lớp bọc OSAL đa nền tảng (chạy native POSIX/Windows trên Host và FreeRTOS trên MCU).

### 2.6. Tầng 6: Platform Layer (`platform/`)
- **Ngoại vi Bus dùng chung nội bộ Platform (`platform/common/inc/`)**:
  - [`platform/common/inc/can_interface.h`](platform/common/inc/can_interface.h): Hợp đồng CAN/CAN-FD bus driver.
  - [`platform/common/inc/uart_interface.h`](platform/common/inc/uart_interface.h): Hợp đồng UART bus driver.
- **Phần cứng thật STM32H7A3ZIQ (`platform/stm32h7a3ziq/`)**:
  - [`platform.cmake`](platform/stm32h7a3ziq/platform.cmake): Nạp nguồn HAL, FreeRTOS và Linker Script.
  - [`board/board.c`](platform/stm32h7a3ziq/board/board.c): Board Support Package / IoHwAb, khởi tạo driver cụ thể và bind vào `board_hardware_t`.
  - [`drivers/tmc2209_driver.c/.h`](platform/stm32h7a3ziq/drivers/tmc2209_driver.c): Driver động cơ bước UART.
  - [`drivers/bldc_can_driver.c/.h`](platform/stm32h7a3ziq/drivers/bldc_can_driver.c): Driver động cơ không chổi than CAN.
  - [`drivers/brake_driver.c/.h`](platform/stm32h7a3ziq/drivers/brake_driver.c): Driver phanh điện từ GPIO.
  - [`drivers/operator_input_driver.c/.h`](platform/stm32h7a3ziq/drivers/operator_input_driver.c): Driver Joystick ADC DMA & Nút bấm GPIO.
  - [`mcal/uart_mcal.c/.h`](platform/stm32h7a3ziq/mcal/uart_mcal.c): MCAL ngoại vi UART STM32 HAL & Console adapter.
  - [`mcal/can_mcal.c/.h`](platform/stm32h7a3ziq/mcal/can_mcal.c): MCAL ngoại vi FDCAN STM32 HAL.
  - [`cubemx/STM32H7A3xx_FLASH.ld`](platform/stm32h7a3ziq/cubemx/STM32H7A3xx_FLASH.ld): Linker script cho STM32H7A3ZI (Flash 2MB, DTCMRAM 128KB, AXI-SRAM 1MB).
  - [`cubemx/startup_stm32h7a3xxq.s`](platform/stm32h7a3ziq/cubemx/startup_stm32h7a3xxq.s): Startup code assembly.
- **Nền tảng Host SIL mô phỏng (`platform/host/`)**:
  - [`platform.cmake`](platform/host/platform.cmake), `board/board.c`, `drivers/`, `mcal/`.

### 2.7. Tầng Kiểm Thử (`tests/`) - 65 Unity Unit Tests & STM32 HAL Stubs
- [`tests/CMakeLists.txt`](tests/CMakeLists.txt): Biên dịch Unity framework thuần C, target `coverage_report` (gcovr HTML).
- [`tests/unity/`](tests/unity/): Thư viện Unity C framework (<10KB footprint).
- [`tests/stubs/stm32_hal_stub.h/.c`](tests/stubs/stm32_hal_stub.h): STM32 HAL Hardware Stubs (GPIO, UART, ADC DMA).
- [`tests/drivers/`](tests/drivers/): Kiểm thử trực tiếp 4 driver phần cứng thật STM32 (BLDC CAN, TMC2209, Brake, Operator Input).
- [`tests/services/`](tests/services/): Kiểm thử nghiệp vụ Motor Service, Brake Service, Operator Service.
- [`tests/connectivity/`](tests/connectivity/): Kiểm thử giải mã chuỗi CLI protocol.
- [`tests/app/`](tests/app/): Kiểm thử System Coordinator & Dependency Injection.
- [`tests/middleware/freertos/`](tests/middleware/freertos/): Kiểm thử FreeRTOS OSAL.

### 2.8. File Cấu Hình Dự Án Cốt Lõi (Root) & Rules
- [`CMakeLists.txt`](CMakeLists.txt): CMake điều phối toàn dự án.
- [`CMakePresets.json`](CMakePresets.json): Cấu hình 5 preset thống nhất (Windows: `stm32h7`, `host-tests`, `coverage`; Linux/Docker: `linux-stm32h7`, `linux-coverage`).
- [`arm-none-eabi-gcc.cmake`](arm-none-eabi-gcc.cmake): Toolchain file biên dịch chéo ARM GNU.
- [`docker/Dockerfile`](docker/Dockerfile): Image Docker chuẩn hóa Ubuntu 22.04, ARM GCC 13.3, GCC 12, CMake, Ninja, gcovr.
- [`docker-compose.yml`](docker-compose.yml): Rút gọn lệnh build container và quản lý volume.
- [`.devcontainer/devcontainer.json`](.devcontainer/devcontainer.json): VS Code Dev Container cấu hình môi trường phát triển 1-click.
- [`AGENTS.md`](AGENTS.md): Bản đồ nguyên tắc Persona và Điều Phối Tự Trị.
- [`.agent/rules/autonomous_orchestration.md`](.agent/rules/autonomous_orchestration.md): Quy chuẩn tự trị và phê duyệt nội bộ workspace.
- [`.agent/skills/autonomous-orchestration/SKILL.md`](.agent/skills/autonomous-orchestration/SKILL.md): Kỹ năng điều phối multi-agent và self-approval.
- [`.vscode/tasks.json`](.vscode/tasks.json): Tasks biên dịch tự động.
- [`.vscode/settings.json`](.vscode/settings.json): Cấu hình cortex-debug và toolchain paths.
- [`README.md`](README.md): Tài liệu hướng dẫn chính thức.

---

## 3. Quy Tắc Ánh Xạ Kiến Trúc 1-1 (Zero-Search Rule)

Khi cần tìm hoặc sửa một tính năng, luôn tra cứu theo bảng ánh xạ 1-1 sau:

| Nghiệp vụ (Feature) | Domain Service (`services/`) | Abstract Interface (`interfaces/`) | Driver phần cứng (`platform/stm32h7a3ziq/drivers/`) |
| :--- | :--- | :--- | :--- |
| **Động cơ 4 trục** | `motor_service.c/.h` | `motor_interface.h` | `tmc2209_driver.c` (UART), `bldc_can_driver.c` (CAN) |
| **Phanh cơ khí** | `brake_service.c/.h` | `brake_interface.h` | `brake_driver.c` (GPIO) |
| **Nhập liệu Joy/Nút** | `operator_service.c/.h` | `operator_input_interface.h` | `operator_input_driver.c` (ADC DMA & GPIO) |
| **Terminal / CLI** | `cli_service.c/.h` | `console_interface.h` | `uart_mcal.c` (Console Stream wrapper) |
| **Hệ thống Board** | `system_coordinator.c/.h` | `board_interface.h` | `board/board.c` (BSP IoHwAb) |
| **CAN Bus Hardware**| *(Nội bộ Platform)* | `platform/common/inc/can_interface.h` | `mcal/can_mcal.c` |
| **UART Bus Hardware**| *(Nội bộ Platform)* | `platform/common/inc/uart_interface.h`| `mcal/uart_mcal.c` |


