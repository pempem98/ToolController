# TIÊU CHUẨN COMMIT VÀ QUY TRÌNH GIT (CONVENTIONAL COMMITS)

## 1. Cấu trúc Message Commit

Mọi commit bắt buộc tuân theo định dạng:
```text
<type>(<scope>): <mô tả ngắn gọn thì hiện tại, không viết hoa chữ cái đầu, không dấu chấm cuối>

[Thân commit - Giải thích TẠI SAO và CHI TIẾT THAY ĐỔI (optional nhưng khuyến khích)]

[Footer: BREAKING CHANGE, Closes #issue, Co-authored-by (optional)]
```

---

## 2. Commit Types

| Type | Mục đích | Ví dụ |
| :--- | :--- | :--- |
| **`feat`** | Tính năng mới cho firmware, driver hoặc service | `feat(canopen): add support for PDO asynchronous transmission` |
| **`fix`** | Sửa lỗi bug, crash, memory leak, race condition | `fix(stm32h7): resolve DMA D-Cache coherency issue in UART RX` |
| **`refactor`** | Tái cấu trúc code mà không thay đổi hành vi logic | `refactor(motion_service): modularize PID computation logic` |
| **`perf`** | Tối ưu hóa hiệu năng, giảm thời gian thực thi ISR/WCET | `perf(dsp): optimize vector multiplication using CMSIS-DSP SIMD` |
| **`test`** | Thêm mới hoặc cập nhật Unit Test, Mock, HIL/SIL tests | `test(io_service): add boundary test cases for analog input channels` |
| **`docs`** | Cập nhật tài liệu kỹ thuật, Doxygen, README, Architecture specs | `docs(ethercat): document state transition diagram and timing constraints` |
| **`chore`** | Các tác vụ phụ trợ, quản lý build, cập nhật cấu hình | `chore(cmake): update ARM GCC compiler optimization flags to -O2` |
| **`ci`** | Cập nhật workflow CI/CD, kịch bản test tự động | `ci(github-actions): add automated gcov code coverage check step` |

---

## 3. Quy chuẩn Scope cho Dự án Embedded

Scope phải thể hiện rõ phân tầng kiến trúc hoặc module phần cứng bị ảnh hưởng:
- **Platform / Drivers**: `(stm32h7)`, `(sim)`, `(gpio)`, `(can)`, `(uart)`, `(timer)`, `(dma)`
- **Interfaces**: `(interfaces/gpio)`, `(interfaces/can)`, `(interfaces/wdt)`
- **Middleware**: `(freertos)`, `(canopen)`, `(ethercat)`
- **Services**: `(motion_service)`, `(health_monitor)`, `(fieldbus_service)`, `(io_service)`
- **App**: `(coordinator)`, `(app_config)`, `(tasks)`

---

## 4. Ví dụ Commit Chuẩn Mực

### Ví dụ 1: Sửa lỗi ngoại lệ DMA Cache
```text
fix(stm32h7/dma): invalidate D-Cache before processing UART RX buffer

The Cortex-M7 core was reading stale data from D-Cache instead of
newly arrived bytes written by DMA in AXI SRAM. 
Added SCB_InvalidateDCache_by_Addr() call right before triggering RX callback.
```

### Ví dụ 2: Thêm tính năng Service
```text
feat(motion_service): integrate S-curve profile trajectory planner

Implement jerk-limited 7-segment motion trajectory algorithm.
Reduces mechanical vibration during high-speed positioning.
```

