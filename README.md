# ToolController - Embedded Software Architecture

Tài liệu hướng dẫn chi tiết dành cho Lập trình viên nhúng (Developer Guide) về kiến trúc, cách xây dựng (build), bảo trì và phát triển/mở rộng mã nguồn cho dự án **ToolController**.

---

## 📌 1. Kiến trúc Tổng quan (Clean Architecture)

Dự án được thiết kế theo mô hình **Clean Architecture / Layered Architecture**, tách biệt hoàn toàn giữa **Nghiệp vụ ứng dụng (Business Logic)** và **Phần cứng cụ thể (Hardware Drivers)**.

```text
+-----------------------------------------------------------------------+
|                         APP LAYER (app/)                              |
|           main_app.c, system_coordinator.c, rtos_tasks.c             |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                      SERVICES LAYER (services/)                       |
|   motion_service   |   health_monitor   |   fieldbus   |  io_service  |
|          (100% C/C++ Thuần - Không phụ thuộc vi điều khiển)           |
+-----------------------------------------------------------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                     INTERFACES LAYER (interfaces/)                    |
|   can_interface  |  gpio_interface  |  wdt_interface  | sensor_if...  |
|                   (Hợp đồng chuẩn / Abstract APIs)                    |
+-----------------------------------------------------------------------+
                                   ^
                                   | (Impl via Driver Adapters)
+-----------------------------------------------------------------------+
|                     PLATFORM LAYER (platform/)                        |
|  platform/stm32f4/   |   platform/stm32h7/   |   platform/nxp_s32k/   |
| (CubeMX / HAL SDK)   |   (CubeMX / HAL SDK)  | (S32DS / MCAL Drivers) |
+-----------------------------------------------------------------------+
```

---

## 📁 2. Cấu trúc Thư mục Dự án

```text
ToolController/
├── app/                        # Mã nguồn ứng dụng cao nhất (Application Coordination)
│   ├── inc/                    # Header của App (main_app.h, system_coordinator.h,...)
│   ├── src/                    # Chứa main_app.c, system_coordinator.c, rtos_tasks.c
│   └── CMakeLists.txt          # File cấu hình build chính ra executable (.elf, .bin, .hex)
├── interfaces/                 # Định nghĩa các chuẩn giao tiếp (C Struct Function Pointers)
│   ├── can_interface.h         # Chuẩn CAN/CAN-FD
│   ├── gpio_interface.h        # Chuẩn GPIO (read, write, toggle, interrupt)
│   ├── wdt_interface.h         # Chuẩn Watchdog
│   ├── sensor_interface.h     # Chuẩn Cảm biến
│   └── ethercat_interface.h   # Chuẩn EtherCAT
├── services/                   # Các dịch vụ logic nghiệp vụ (Thuần logic, độc lập chip)
│   ├── motion_service/         # Quản lý Động cơ, CiA 402 State Machine, CANopen/EtherCAT CoE
│   ├── health_monitor/         # Task Watchdog Manager & Heartbeat Service
│   ├── fieldbus_service/       # Điều hướng gói tin Fieldbus (Fieldbus Router)
│   └── io_service/             # Xử lý Phanh (Brake Controller) & Cảm biến (Sensor Processor)
├── platform/                   # Lớp driver và mã sinh tự động theo từng dòng chip
│   └── stm32f4/                # Nền tảng STM32F4 (F407VET6)
│       ├── bsp/                # BSP Pins (`bsp_pins.h`), `FreeRTOSConfig.h`
│       ├── cubemx/             # Thư viện sinh tự động từ STM32CubeMX (Core, Drivers, .ioc)
│       └── driver_adapter/     # Mã chuyển đổi (Adapter) từ HAL ST sang interfaces chuẩn
├── tests/                      # Unit test chạy trên PC (Host PC build qua GoogleTest)
├── arm-none-eabi-gcc.cmake     # File cấu hình Toolchain biên dịch chéo ARM GNU
├── CMakeLists.txt              # CMake điều khiển gốc toàn dự án
└── README.md                   # Tài liệu này
```

---

## 🚀 3. Hướng dẫn Biên dịch (Building the Project)

### Yêu cầu Tiền đề (Prerequisites)
1. **Toolchain**: `arm-none-eabi-gcc` (Đi kèm trong STM32CubeIDE hoặc tải từ Arm GNU Toolchain).
2. **Build Tools**: `CMake` (v3.20+) và `Ninja` (hoặc `MinGW Makefiles`).

### Các bước biên dịch:

#### 1. Biên dịch Firmware cho STM32F4 (Cross-Compilation)
Mở Terminal (PowerShell hoặc Bash) tại thư mục gốc dự án:

```powershell
# Thêm đường dẫn chứa arm-none-eabi-gcc vào PATH (nếu chưa thêm vào Windows Environment)
$env:PATH = "C:\Users\m920q\AppData\Local\stm32cube\bundles\gnu-tools-for-stm32\14.3.1+st.2\bin;" + $env:PATH

# Khởi tạo thư mục build với Toolchain ARM
cmake -B build "-DCMAKE_TOOLCHAIN_FILE=arm-none-eabi-gcc.cmake" -G Ninja

# Tiến hành biên dịch
cmake --build build
```

#### 2. Kết quả xuất ra
Sau khi build thành công, các file thực thi nằm tại thư mục `build/app/`:
* `my_embedded_project_app.elf` — Dùng để Debug với ST-Link / J-Link trong VS Code hoặc IDE.
* `my_embedded_project_app.bin` — File nhị phân nạp bằng STM32CubeProgrammer hoặc Bootloader.
* `my_embedded_project_app.hex` — File Intel HEX cho các mạch nạp tiêu chuẩn.

---

## 🛠️ 4. Quy trình Bảo trì & Mở rộng (Maintainer Workflows)

### Tình huống 1: Cần cấu hình lại Pinout / Clock / Ngoại vi trên STM32CubeMX
1. Mở file `platform/stm32f4/cubemx/cubemx.ioc` bằng **STM32CubeMX**.
2. Thay đổi cấu hình mong muốn (ví dụ: đổi pin, bật thêm SPI/UART).
3. Bấm **Generate Code**.
4. **Không cần sửa gì thêm trong `main.c`**: Lệnh gọi `App_Main()` được bảo vệ bên trong khối `/* USER CODE BEGIN 2 */` nên CubeMX sẽ không xóa mất.
5. Nếu bật thêm ngoại vi mới (VD: `spi.c`), hãy thêm tên file `.c` đó vào mục `add_executable` trong `app/CMakeLists.txt`.

### Tình huống 2: Viết thêm Logic nghiệp vụ mới (Ví dụ: Service mới)
1. Tạo thư mục service mới trong `services/my_new_service/`.
2. Tạo `CMakeLists.txt` bên trong dạng `add_library(my_new_service STATIC ...)` link tới `core_interfaces`.
3. Thêm `add_subdirectory(services/my_new_service)` vào file `CMakeLists.txt` gốc.
4. Link `my_new_service` vào `app/CMakeLists.txt`.

### Tình huống 3: Chuyển sang Dòng chip khác (NXP, TI C2000, STM32H7)
1. Tạo thư mục nền tảng mới: `platform/nxp_s32k/` hoặc `platform/stm32h7/`.
2. Generate SDK/Code của hãng đó vào trong thư mục đó.
3. Viết các Adapter trong `platform/<mcu_name>/driver_adapter/` bọc hàm SDK của chip đó sang `interfaces` chuẩn (`can_interface`, `gpio_interface`,...).
4. Tầng `app/` và `services/` **giữ nguyên 100% mã nguồn**, không cần sửa dòng code nào.

---

## 📝 5. Luồng Chạy Mã Nguồn (Execution Flow)

1. **Khởi động vi điều khiển**: `startup_stm32f407xx.s` -> Gọi `main()` trong `platform/stm32f4/cubemx/Core/Src/main.c`.
2. **Khởi tạo HAL phần cứng**: `HAL_Init()` -> `SystemClock_Config()` -> `MX_GPIO_Init()` -> `MX_CAN1_Init()`.
3. **Chuyển tiếp sang App**: Tại `USER CODE BEGIN 2`, gọi `App_Main()` trong `app/src/main_app.c`.
4. **Khởi tạo Coordinator**: `system_coordinator_init()` tạo các driver adapter tương ứng cho F4 và tiêm (inject) vào các Service.
5. **Khởi chạy RTOS**: `rtos_start_scheduler()` bắt đầu điều phối các Task định kỳ (Motion task, Fieldbus task, IO task, Watchdog task).

---

## 🤝 6. Quy chuẩn Viết Code (Coding Guidelines)

* **Không dính HAL/Hardware vào Services**: Trong `services/`, tuyệt đối không `#include "stm32f4xx_hal.h"`. Mọi giao tiếp với phần cứng phải qua các header định nghĩa trong `interfaces/`.
* **Tránh dùng biến toàn cục tự do**: Gom các instance phần cứng và service vào struct quản lý chính `system_coordinator_t g_sys`.
* **Khai báo Header sạch**: Mọi hàm entry point phải khai báo prototype trong file `.h` tương ứng (Ví dụ: `void App_Main(void);` trong `main_app.h`).
