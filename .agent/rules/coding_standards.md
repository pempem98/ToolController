# TIÊU CHUẨN MÃ NGUỒN & CODING STYLE (EMBEDDED C/C++)

## 1. Naming Conventions (Quy ước Đặt tên)

| Đối tượng | Quy ước | Ví dụ | Ghi chú |
| :--- | :--- | :--- | :--- |
| **Files** | `snake_case` | `canopen_node.c`, `gpio_interface.h` | Header `.h`, Source `.c`, C++ `.hpp`/`.cpp` |
| **Functions** | `snake_case` (prefix theo module) | `system_coordinator_init()`, `can_adapter_send()` | Định danh rõ ràng theo module/layer |
| **Types / Struct / Enum** | `snake_case_t` | `gpio_state_t`, `canopen_node_t` | Luôn có hậu tố `_t` |
| **Enum Values** | `UPPER_SNAKE_CASE` (prefix theo enum) | `GPIO_STATE_LOW`, `CAN_ERR_BUS_OFF` | Tránh xung đột tên |
| **Macros / Constants** | `UPPER_SNAKE_CASE` | `MAX_PAYLOAD_SIZE`, `BUFFER_ALIGN_SIZE` | Không dùng magic numbers |
| **Global Variables** | `g_` + `snake_case` | `g_sys`, `g_canopen_node` | Hạn chế tối đa; bắt buộc có mutex/critical section nếu đa luồng |
| **Static / File Variables** | `s_` + `snake_case` | `s_tx_buffer`, `s_is_initialized` | Luôn khai báo `static` để giới hạn phạm vi file |
| **Pointers** | `p_` hoặc `snake_case` | `p_buffer`, `p_self` | Khuyến khích `const` correctness |
| **Interfaces / Callbacks** | `*_interface_t`, `*_callback_t` | `gpio_interface_t`, `can_rx_callback_t` | Thiết kế hướng đối tượng C qua vtable/function pointers |

---

## 2. Structure & File Rules (Quy chuẩn Cấu trúc File & Kiến trúc)

### 2.1. Cấu trúc chuẩn của Header File (`.h`)
```c
#ifndef MODULE_NAME_H
#define MODULE_NAME_H

/* 1. Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 2. Project / Interface Includes */
#include "interfaces/os_interface.h"

/* 3. C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/* 4. Macros & Constants */
#define MODULE_MAX_RETRIES (3U)

/* 5. Type Definitions (Enums, Structs, Function Pointers) */
typedef enum {
    MODULE_STATUS_OK = 0,
    MODULE_STATUS_ERR_TIMEOUT,
    MODULE_STATUS_ERR_PARAM
} module_status_t;

/* 6. Public Function Prototypes */
module_status_t module_init(void);

#ifdef __cplusplus
}
#endif

#endif /* MODULE_NAME_H */
```

### 2.2. Thứ tự `#include` trong Source File (`.c`)
1. Header tương ứng của chính file đó (`"my_module.h"`).
2. Standard C/C++ library headers (`<stdint.h>`, `<string.h>`).
3. OSAL / Middleware / Interface headers (`"interfaces/..."`, `"middleware/..."`).
4. Drivers / Platform headers (`"platform/..."`).

### 2.3. Nguyên tắc Phân tầng (Layered Architecture Rules)
- **App** gọi **Services** và **Interfaces**. Không truy cập trực tiếp thanh ghi / platform driver.
- **Services** chỉ phụ thuộc vào **Interfaces** và **Middleware**. Hoàn toàn độc lập với phần cứng MCU cụ thể (chạy được cả trên Host Simulation `sim` và Hardware `stm32h7`).
- **Interfaces** thuần C structs chứa con trỏ hàm (Function Pointers) để trừu tượng hóa phần cứng và hỗ trợ Mocking trong Unit Test.
- **Platform/Drivers** triển khai các con trỏ hàm trong **Interfaces**.

---

## 3. Comment & Documentation Format (Chuẩn Doxygen)

Mọi hàm công khai (public API), struct, enum và macro quan trọng bắt buộc phải có Doxygen comment chuẩn:

```c
/**
 * @brief  Khởi tạo module truyền thông CANopen với cấu hình chỉ định.
 * 
 * @param[in,out] p_node  Con trỏ tới đối tượng canopen_node_t cần khởi tạo.
 * @param[in]     node_id Địa chỉ Node ID (1 - 127).
 * @param[in]     baud    Tốc độ baud rate của bus.
 * 
 * @return status_t
 *   - STATUS_OK:              Khởi tạo thành công.
 *   - STATUS_ERR_INVALID_ARG: Tham số NULL hoặc Node ID ngoài dải hợp lệ.
 * 
 * @note   Hàm này không thread-safe, phải được gọi trong pha khởi tạo hệ thống trước khi start scheduler.
 * @warning Bắt buộc đảm bảo clock CAN peripheral đã được cấp trước khi gọi.
 */
status_t canopen_init(canopen_node_t *p_node, uint8_t node_id, uint32_t baud);
```

- **Inline comments**: Sử dụng `//` giải thích *TẠI SAO* (Why/Intent) thay vì *LÀM GÌ* (What).
- **Todo/Fixme format**: `// TODO(engineer_name): Lý do cần hoàn thiện` hoặc `// FIXME: Mô tả lỗi cần xử lý`.

---

## 4. An Toàn Bộ Nhớ & Embedded Rules (MISRA-C / Best Practices)

1. **Explicit Integer Types**: Luôn sử dụng `<stdint.h>` (`uint8_t`, `int16_t`, `uint32_t`, `uint64_t`), cấm dùng `int`, `short`, `long` không chỉ định kích thước.
2. **Const Correctness**: Sử dụng `const` cho tất cả con trỏ chỉ đọc (`const uint8_t *p_data`, `const char *str`).
3. **No Dynamic Allocation in Loops**: Cấm `malloc()`, `free()`, `new`, `delete` trong các vòng lặp thời gian thực và ISR. Sử dụng static allocation hoặc memory pool định kích thước trước.
4. **Volatile Usage**: Bắt buộc sử dụng `volatile` cho các biến cờ chia sẻ giữa ISR và Main thread, cũng như con trỏ tới thanh ghi phần cứng (Hardware Registers).
5. **Array Boundary & Buffer Safety**: Luôn truyền kèm tham số độ dài bộ đệm (`size_t len` / `size_t max_len`). Tuyệt đối không dùng `strcpy`, `sprintf`; thay bằng `strncpy`, `snprintf`.
6. **DMA Cache Alignment**: Các buffer DMA trên ARM Cortex-M7 phải được căn lề 32-byte (`__attribute__((aligned(32)))`) và invalidate/clean D-Cache đúng quy trình.

