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
| **`fix`** | Sửa lỗi bug, crash, memory leak, race condition | `fix(stm32h7a3zit6q): resolve DMA D-Cache coherency issue in UART RX` |
| **`docs`** | Thêm / cập nhật tài liệu, comment Doxygen, README | `docs(readme): add wiring diagram for TMC2209 stepper driver` |
| **`style`** | Định dạng code (whitespace, format, missing semi-colons) | `style(services): apply clang-format rules to motor_service` |
| **`refactor`** | Tái cấu trúc code (không sửa bug, không thêm tính năng) | `refactor(interfaces): split monolithic bus interface into CAN and UART` |
| **`test`** | Thêm / sửa unit tests, mock, SIL tests | `test(brake_service): add test cases for emergency brake timeout` |
| **`chore`** | Cập nhật cấu hình build, toolchain, gitignore, CI/CD | `chore(cmake): bump minimum required version to 3.22` |
| **`perf`** | Tối ưu hóa hiệu năng, giảm chu kỳ CPU, tiết kiệm RAM | `perf(math): replace floating-point trigonometry with LUT` |
| **`ci`** | Cập nhật GitHub Actions, pipeline CI | `ci(github): add workflow for automated host unit testing` |

---

## 3. Quy Tắc Scope (Phạm Vi Thay Đổi)
Scope phải chỉ rõ module hoặc subsystem bị ảnh hưởng:
- **Application**: `(app)`, `(main)`, `(state_machine)`, `(coordinator)`
- **Domain Services**: `(motor_service)`, `(brake_service)`, `(operator_service)`, `(cli_service)`
- **Interfaces**: `(interfaces)`, `(motor_if)`, `(brake_if)`, `(sensor_if)`
- **Connectivity**: `(cli)`, `(canopen)`, `(modbus)`
- **Middleware**: `(freertos)`, `(osal)`, `(queue)`, `(mutex)`
- **Platform / Drivers**: `(stm32h7a3zit6q)`, `(sim)`, `(gpio)`, `(can)`, `(uart)`, `(timer)`, `(dma)`
- **Toolchain / Build**: `(cmake)`, `(presets)`, `(toolchain)`, `(linker)`, `(vscode)`
- **Testing**: `(unity)`, `(sil)`, `(mocks)`, `(coverage)`

---

## 4. Quy Tắc Viết Subject (Tiêu Đề Commit)
1. **Dùng thể mệnh lệnh, thì hiện tại** (Imperative mood): *"add"*, *"fix"*, *"refactor"*, *"update"*, KHÔNG dùng *"added"*, *"fixes"*, *"refactored"*.
2. **Không viết hoa chữ cái đầu** của subject (trừ tên riêng như `FreeRTOS`, `DMA`).
3. **Không có dấu chấm `.` ở cuối câu**.
4. **Độ dài tối đa 72 ký tự** cho dòng đầu tiên.
5. **Ngôn ngữ**: Khuyến khích viết bằng **tiếng Anh kỹ thuật** chuẩn mực.

---

## 5. Ví Dụ Commit Chuẩn Mẫu

```text
feat(motor_service): implement S-curve acceleration profile for BLDC motors
```

```text
fix(stm32h7a3zit6q/dma): invalidate D-Cache before processing UART RX buffer
```
