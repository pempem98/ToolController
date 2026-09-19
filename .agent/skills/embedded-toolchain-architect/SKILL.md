---
name: embedded-toolchain-architect
description: >-
  Expertise in Embedded & Cross-Platform Toolchain Configuration, Environment Detection, and IDE Orchestration. Use when setting up or migrating embedded environments across OS (Windows, Linux/Ubuntu, macOS, WSL), detecting and configuring compilers (ARM GNU GCC, Clang/LLVM, host GCC/G++), build systems (CMake, CMakePresets, Ninja, Make), debug probes & GDB servers (ST-LINK, OpenOCD, J-Link), or generating clean, portable .vscode configurations (settings.json, tasks.json, launch.json) without hardcoded paths.
---

# Embedded Toolchain & Environment Architect Skill

## 1. Core Competencies

- **Multi-OS Host Environments**:
  - **Windows**: Handling `%LOCALAPPDATA%`, ST bundles, STM32CubeCLT/IDE, PATH separators (`;`), shell environments (`cmd.exe`, `powershell`).
  - **Linux / Ubuntu**: Package manager installs (`gcc-arm-none-eabi`, `gdb-multiarch`, `ninja-build`, `openocd`, `stlink-tools`), `/opt/st/` paths, Linux permissions & `udev` rules (`/etc/udev/rules.d/`) for USB debuggers (ST-Link V2/V3, J-Link).
  - **WSL2**: USB/IP device passthrough (`usbipd wsl`), cross-environment file access.
  - **macOS**: Homebrew toolchains (`arm-none-eabi-gcc`), Apple Clang vs GCC flags.

- **Compilers & Cross-Toolchains**:
  - **Target Cross-Compilers**: ARM GNU Toolchain (`arm-none-eabi-gcc`, `arm-none-eabi-g++`, `arm-none-eabi-objcopy`, `arm-none-eabi-size`, `arm-none-eabi-gdb` / `gdb-multiarch`).
  - **Host / SIL Compilers**: Native GCC/G++, Clang/LLVM, MSVC for running unit tests, coverage, and Software-in-the-Loop simulation without MCU hardware.
  - **Clang/LLVM for Embedded**: Cross-compiling with `-target arm-none-eabi`, `-mfpu=...`, `--sysroot`.

- **Build Systems & Automation**:
  - **CMake**: Toolchain files, dynamic compiler discovery (`find_program`, `file(GLOB ...)`), `CMakePresets.json` (configure, build, test presets).
  - **Generators**: Ninja (fast parallel builds), Unix Makefiles.
  - **Automated Validation**: Target `.elf` / `.bin` / `.hex` generation, size analysis, map file inspection.

- **Hardware Debuggers & GDB Servers**:
  - **ST-LINK V2/V3**: ST-LINK GDB Server (`ST-LINK_gdbserver`), STM32CubeProgrammer CLI (`STM32_Programmer_CLI`), OpenOCD (`interface/stlink.cfg`, `target/stm32h7x.cfg`).
  - **Segger J-Link**: `JLinkGDBServerCL`, `JLinkExe`.
  - **OpenOCD**: Universal open-source GDB server (essential for headless Linux / CI / Ubuntu development).

- **IDE Orchestration (`.vscode`)**:
  - `.vscode/settings.json`: Environment variables, minimal non-redundant settings, no user-specific hardcoded paths.
  - `.vscode/tasks.json`: Multi-OS tasks (`windows`, `linux`, `osx`), default build/clean/rebuild tasks.
  - `.vscode/launch.json`: Cortex-Debug configurations, SVD peripheral register mapping, SWD interface, reset types.

---

## 2. Bài Học Kinh Nghiệm Thực Chiến (Lessons Learned)

### A. Phân Biệt Cross-Compiler vs Host Compiler

> **CRITICAL**: `arm-none-eabi-gcc` là Cross-Compiler — nó sinh mã máy ARM chạy trên chip MCU. Nó KHÔNG THỂ biên dịch file thực thi chạy trên Host PC (x86_64 Windows/Linux). Khi cần chạy Unit Test (GoogleTest) hoặc SIL trên Host PC, bắt buộc phải có Host Compiler riêng biệt.

| Mục đích | Compiler cần dùng | Giải thích |
| :--- | :--- | :--- |
| Build firmware STM32 | `arm-none-eabi-gcc` | Cross-compiler sinh nhị phân ARM Cortex-M |
| Chạy Unit Tests trên PC | `gcc`/`g++` (MinGW trên Windows, native trên Linux) | Host compiler sinh file .exe (Windows) hoặc ELF x86_64 (Linux) |
| Đo Code Coverage | `gcc` + `gcov` + `lcov` | `gcov` chỉ hoạt động với GCC, không phải Clang |

### B. LLVM Clang Trên Windows: Bẫy Ngầm

- LLVM Clang cài qua installer mặc định trên Windows có `Target: x86_64-pc-windows-msvc`.
- Nó cố link bằng `lld-link` (MSVC linker), yêu cầu Windows SDK (`kernel32.lib`, `msvcrtd.lib`).
- **Nếu máy Windows chưa cài Visual Studio Build Tools**: Clang KHÔNG THỂ link thành file .exe.
- **Hệ quả**: CMake Tools extension tự phát hiện Clang, cố dùng để biên dịch test project, dẫn đến lỗi `could not open 'kernel32.lib'`.
- **Giải pháp**: Ép toolchain file `arm-none-eabi-gcc.cmake` trước `project()` trong `CMakeLists.txt`:
  ```cmake
  if(NOT CMAKE_TOOLCHAIN_FILE AND NOT PLATFORM STREQUAL "sim" AND NOT BUILD_TESTS)
      include("${CMAKE_CURRENT_LIST_DIR}/arm-none-eabi-gcc.cmake")
  endif()
  ```

### C. PowerShell 5.1 Không Hỗ Trợ Toán Tử `&&`

- VS Code trên Windows mặc định chạy task qua PowerShell.
- PowerShell 5.1 (có sẵn trên Windows 10/11) KHÔNG hỗ trợ cú pháp `&&` để nối lệnh tuần tự.
- Lỗi: `ParserError: The token '&&' is not a valid statement separator in this version`.
- **Giải pháp**: Cấu hình shell cho Windows trong `tasks.json`:
  ```json
  "windows": {
      "options": {
          "shell": {
              "executable": "cmd.exe",
              "args": ["/d", "/c"]
          }
      }
  }
  ```

### D. Nguyên Tắc Kiến Trúc File Cấu Hình VS Code

**Quy tắc "Single Source of Truth" cho đường dẫn tuyệt đối**:

1. **`settings.json`** là NƠI DUY NHẤT được phép chứa absolute/host-specific paths:
   - `cortex-debug.stlinkPath`, `cortex-debug.stm32cubeprogrammer`, `cortex-debug.armToolchainPath`.
   - `terminal.integrated.env.windows` (nạp PATH cho mọi task).
   - `cmake.environment` (nạp PATH cho CMake Tools extension).

2. **`launch.json`** phải 100% relative workspace paths:
   - `${workspaceFolder}/build/app/...` cho `executable` và `svdFile`.
   - Cortex-Debug tự động đọc probe path từ `settings.json`.

3. **`tasks.json`** phải 100% relative:
   - Dùng `cmake --preset ...` thay vì inline dài dòng.
   - Không lặp lại PATH cục bộ trong từng task.

4. **`CMakePresets.json`** phải 100% relative:
   - Dùng `${sourceDir}/...` cho mọi đường dẫn.

### E. Không Lưu File Thực Thi Portable Trong Repo

- **KHÔNG** lưu trình biên dịch, GDB server, hay công cụ nhị phân vào `tools/` của dự án.
- Lý do:
  1. File `.exe` / `.dll` chỉ chạy trên 1 OS, không tương thích cross-platform.
  2. Gây bloat repo (>1GB nếu bao gồm cả ST-Link + Programmer).
  3. Driver phần cứng (USB WinUSB/udev) vẫn cần cài đặt riêng.
- **Nguyên tắc**: Repo chỉ chứa mã nguồn + build scripts. Toolchain do từng máy tự quản lý.

### F. Post-Build Tool Resolution (`objcopy`, `size`)

- Biến `CMAKE_OBJCOPY` và `CMAKE_SIZE` có thể bị mất hoặc rỗng trong cache nếu không được FORCE.
- **Giải pháp**: Trong `app/CMakeLists.txt`, tự tìm lại từ thư mục chứa compiler:
  ```cmake
  get_filename_component(TOOLCHAIN_BIN_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
  find_program(OBJCOPY_BIN arm-none-eabi-objcopy HINTS "${TOOLCHAIN_BIN_DIR}")
  find_program(SIZE_BIN arm-none-eabi-size HINTS "${TOOLCHAIN_BIN_DIR}")
  ```

### G. Xung Đột Cache Khi Dùng Chung Duy Nhất Một Thư Mục `build/` & Giải Pháp `--fresh`

- **Hiện tượng**: Khi tất cả presets (`stm32h7`, `host-tests`, `coverage`) cùng xuất ra `${sourceDir}/build`:
  - Chạy `coverage` ghi `ENABLE_COVERAGE=ON` vào `build/CMakeCache.txt`.
  - Chuyển sang `stm32h7` mà không làm sạch cache sẽ khiến cờ `--coverage` bị tiêm vào trình biên dịch `arm-none-eabi-gcc`.
  - Bảng gcov làm phình to `.bss` thêm 125 KB, gây tràn phân vùng `DTCMRAM` đúng 33,560 bytes.
- **Biện pháp phòng vệ 3 lớp bắt buộc**:
  1. *Guard trong `CMakeLists.txt`*: `if(ENABLE_COVERAGE AND (PLATFORM STREQUAL "host" OR BUILD_TESTS))` để cấm tuyệt đối `--coverage` trên MCU.
  2. *Ép biến trong Preset*: Đặt `"ENABLE_COVERAGE": "OFF"` tường minh cho preset `stm32h7`.
  3. *Tự động xóa cache bằng `--fresh`*: Luôn dùng `cmake --fresh --preset <preset_name>` trong tasks để tái tạo cấu hình sạch sẽ khi đổi kiến trúc.

### H. Môi Trường Kiểm Thử Native & Báo Cáo Coverage HTML (`gcovr`)

- **Host Compiler**: MinGW-W64 16.1.0 UCRT POSIX được cài đặt qua WinGet tại:
  `$env:LOCALAPPDATA\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin`.
- **Gcovr HTML Generator**: Cài trong Python Scripts tại:
  `$env:LOCALAPPDATA\Programs\Python\Python313\Scripts\gcovr.exe`.
- **Target `coverage_report`**: Tự động sinh báo cáo HTML chi tiết từng dòng lệnh tại `build/coverage_report/index.html`.

---

## 3. Ma Trận Chuyển Đổi Môi Trường (Migration Matrix)

### A. Chuyển từ Windows sang Ubuntu / Debian Linux

| Thành phần | Windows (Hiện tại) | Ubuntu / Linux tương đương |
| :--- | :--- | :--- |
| **Cross-Compiler** | `${env:LOCALAPPDATA}/stm32cube/bundles/gnu-tools...` | `/usr/bin/arm-none-eabi-gcc` (`sudo apt install gcc-arm-none-eabi`) |
| **Host Compiler** | MinGW `gcc`/`g++` (`winget install BrechtSanders.winlibs-llvm-ucrt`) | `/usr/bin/gcc` (`sudo apt install build-essential`) |
| **GDB** | `arm-none-eabi-gdb.exe` | `/usr/bin/gdb-multiarch` (`sudo apt install gdb-multiarch`) |
| **Build & Generator** | `cmake.exe`, `ninja.exe` (ST bundles) | `/usr/bin/cmake`, `/usr/bin/ninja` (`sudo apt install cmake ninja-build`) |
| **GDB Server** | `ST-LINK_gdbserver.exe` | OpenOCD (`sudo apt install openocd`) |
| **Coverage** | `gcov.exe` (kèm MinGW GCC) | `gcov` + `lcov` + `genhtml` (`sudo apt install lcov`) |
| **USB Quyền hạn** | Tự nhận driver ST-Link qua WinUSB | Cần cấu hình `udev` rules |

### B. Cài Đặt Nhanh Ubuntu (1 LỆNH DUY NHẤT)

```bash
sudo apt update && sudo apt install -y build-essential gcc-arm-none-eabi libnewlib-arm-none-eabi gdb-multiarch cmake ninja-build openocd lcov
```

### C. Cấu Hình USB ST-Link V3 Trên Ubuntu

```bash
sudo tee /etc/udev/rules.d/49-stlinkv3.rules << 'EOF'
# ST-Link V3
ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374e", MODE="0666", GROUP="plugdev"
ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374f", MODE="0666", GROUP="plugdev"
ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3753", MODE="0666", GROUP="plugdev"
EOF
sudo udevadm control --reload-rules && sudo udevadm trigger
sudo usermod -a -G plugdev $USER
```

### D. Cài Đặt Nhanh Windows (Host Compiler Cho Unit Test)

```powershell
winget install -e --id BrechtSanders.WinLibs.POSIX.UCRT
```
*Gói cung cấp: `gcc`, `g++`, `gcov`, `gdb` (MinGW-w64 UCRT runtime).*

---

## 4. Cấu Hình Chuẩn Portable `.vscode`

### File `settings.json` (NƠI DUY NHẤT chứa absolute paths):
```json
{
    "cmake.generator": "Ninja",
    "cortex-debug.stlinkPath": "<auto-detect-or-ST-install-path>/ST-LINK_gdbserver",
    "cortex-debug.stm32cubeprogrammer": "<auto-detect-or-ST-install-path>/bin",
    "cortex-debug.armToolchainPath": "${env:LOCALAPPDATA}/stm32cube/bundles/gnu-tools-for-stm32/.../bin",
    "cmake.environment": {
        "PATH": "<toolchain-bins>;${env:PATH}"
    },
    "terminal.integrated.env.windows": {
        "PATH": "<toolchain-bins>;${env:PATH}"
    }
}
```

### File `tasks.json` (100% relative, dùng CMake Presets):
```json
{
    "version": "2.0.0",
    "windows": {
        "options": {
            "shell": { "executable": "cmd.exe", "args": ["/d", "/c"] },
            "env": { "PATH": "<toolchain-bins>;${env:PATH}" }
        }
    },
    "tasks": [
        { "label": "Build Firmware", "command": "cmake --preset stm32h7 && cmake --build --preset stm32h7" },
        { "label": "Run Unit Tests", "command": "cmake --preset host-tests && cmake --build --preset host-tests && ctest --preset host-tests" },
        { "label": "Run Tests with Coverage", "command": "cmake --preset coverage && cmake --build --preset coverage && ctest --preset coverage" },
        { "label": "Clean All", "command": "cmake -E rm -rf build build_tests build_coverage" }
    ]
}
```

### File `launch.json` (100% relative):
```json
{
    "configurations": [
        {
            "name": "Debug (ST-Link)",
            "servertype": "stlink",
            "executable": "${workspaceFolder}/build/app/<target>.elf",
            "svdFile": "${workspaceFolder}/platform/<target>/<target>.svd"
        },
        {
            "name": "Debug (OpenOCD - Linux)",
            "servertype": "openocd",
            "configFiles": ["interface/stlink.cfg", "target/stm32h7x.cfg"],
            "executable": "${workspaceFolder}/build/app/<target>.elf"
        }
    ]
}
```

---

## 5. CMake Toolchain File Đa Nền Tảng (Best Practice)

```cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Cross-platform automatic search hints
set(ARM_TOOLCHAIN_HINTS "")
if(WIN32)
    file(GLOB WIN_ST_HINTS "$ENV{LOCALAPPDATA}/stm32cube/bundles/gnu-tools-for-stm32/*/bin")
    file(GLOB WIN_CLT_HINTS "$ENV{SystemDrive}/ST/STM32CubeCLT*/GNU-tools-for-STM32/bin")
    list(APPEND ARM_TOOLCHAIN_HINTS ${WIN_ST_HINTS} ${WIN_CLT_HINTS})
elseif(APPLE)
    list(APPEND ARM_TOOLCHAIN_HINTS "/opt/homebrew/bin" "/usr/local/bin")
elseif(UNIX)
    file(GLOB LINUX_CLT_HINTS "/opt/st/stm32cubeclt*/GNU-tools-for-STM32/bin")
    list(APPEND ARM_TOOLCHAIN_HINTS "/usr/bin" "/usr/local/bin" ${LINUX_CLT_HINTS})
endif()

find_program(ARM_GCC_PATH arm-none-eabi-gcc HINTS ${ARM_TOOLCHAIN_HINTS})
# ... tương tự cho g++, objcopy, objdump, size

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
```

---

## 6. Quy Trình Chẩn Đoán Lỗi Môi Trường Nhanh (Troubleshooting)

1. **Lỗi `spawn ninja ENOENT` hoặc `No usable generator found`**:
   - Kiểm tra PATH trong `cmake.environment` hoặc `terminal.integrated.env.windows`.
   - Trên Linux: `which ninja cmake arm-none-eabi-gcc`.
   - Trên Windows: Kiểm tra thư mục `${env:LOCALAPPDATA}/stm32cube/bundles/`.

2. **Lỗi `The C compiler is not able to compile a simple test program` (Clang + kernel32.lib)**:
   - Nguyên nhân: CMake Tools tự phát hiện LLVM Clang, cố compile Windows test project.
   - Khắc phục: Ép `include("${CMAKE_CURRENT_LIST_DIR}/arm-none-eabi-gcc.cmake")` trước `project()`.

3. **Lỗi `No CMAKE_C_COMPILER could be found` (khi chạy Unit Test trên Windows)**:
   - Nguyên nhân: Chưa cài Host Compiler (MinGW GCC hoặc MSVC).
   - Khắc phục: `winget install -e --id BrechtSanders.winlibs-llvm-ucrt`.

4. **Lỗi `The token '&&' is not a valid statement separator` (PowerShell)**:
   - Nguyên nhân: VS Code dùng PowerShell 5.1 mặc định, không hỗ trợ `&&`.
   - Khắc phục: Cấu hình `"shell": {"executable": "cmd.exe", "args": ["/d", "/c"]}` trong `tasks.json`.

5. **Lỗi `GDB Server Quit Unexpectedly`**:
   - Nguyên nhân: Sai servertype (J-Link cho board ST-Link) hoặc chưa cấp quyền USB udev trên Linux.
   - Khắc phục: Chuyển sang `stlink` hoặc OpenOCD; kiểm tra cáp SWD và cổng USB.

6. **Lỗi `'-O' is not recognized` (post-build objcopy)**:
   - Nguyên nhân: `CMAKE_OBJCOPY` rỗng hoặc không tìm thấy trong PATH.
   - Khắc phục: Dùng `find_program(OBJCOPY_BIN ...)` với HINTS từ `CMAKE_C_COMPILER` directory.

7. **Lỗi `cmake -E rm -rf` không tìm thấy cmake trên Windows**:
   - Nguyên nhân: `cmake` chưa nằm trong PATH hệ thống (chỉ nằm trong ST bundles).
   - Khắc phục: Đảm bảo `terminal.integrated.env.windows` trong `settings.json` nạp PATH đúng.

