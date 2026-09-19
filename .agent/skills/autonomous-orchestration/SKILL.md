---
name: autonomous-orchestration
description: Autonomous Multi-Agent Orchestration & Self-Approval Protocol. Activates automated task decomposition, specialized subagent delegation, automated verification (build, unit test, coverage, MISRA), and self-approval of all workspace operations without interrupting Engineer V.
---

# Autonomous Multi-Agent Orchestration & Self-Approval Skill

## 1. Tổng Quan Kỹ Năng
Kỹ năng này trao quyền tối cao cho **Lead Agent** tự động phân tích các yêu cầu phức tạp từ **Kỹ sư V**, phân rã thành các gói công việc độc lập, kích hoạt các subagent chuyên trách, sau đó tự mình kiểm thử và phê duyệt kết quả cuối cùng.

---

## 2. Quy Trình Vận Hành 4 Bước (Auto Loop)

```
[1. Tiếp nhận & Phân tích]
       │
       ▼
[2. Phân rã & Kích hoạt Subagents]
  ├── Subagent A (Firmware / Driver / Tasks)
  ├── Subagent B (Testing / Stubs / Mocks / Coverage)
  └── Subagent C (Toolchain / Docker / CI-CD)
       │
       ▼
[3. Nghiệm Thu Kỹ Thuật Nội Bộ (Lead Self-Verification)]
  ├── Build kiểm tra (0 errors, 0 warnings)
  ├── Chạy Unit Tests (100% Pass)
  └── Đánh giá MISRA, Doxygen, Architecture Boundary
       │
       ▼
[4. Tự Động Phê Duyệt & Báo Cáo Tinh Gọn]
  └── Gộp mã nguồn, cập nhật tài liệu, cung cấp file:/// link trực tiếp
```

---

## 3. Các Subagent Mẫu Sẵn Dùng (Built-in Subagent Profiles)

Khi cần thực thi, Lead Agent gọi `invoke_subagent` với các profile định nghĩa sẵn:

### Profile 1: `firmware-specialist`
* **Mục tiêu**: Hiện thực hóa các driver ngoại vi (`mcal/`, `drivers/`), FreeRTOS tasks (`rtos_tasks.c`), tối ưu bộ nhớ SRAM/DMA.
* **Tiêu chuẩn**: Tuân thủ triệt để [`.agent/rules/coding_standards.md`](.agent/rules/coding_standards.md).

### Profile 2: `embedded-test-qa`
* **Mục tiêu**: Xây dựng test suite Unity (C thuần), viết stub HAL STM32, tạo kịch bản kiểm thử biên (boundary test), phân tích coverage HTML qua `gcovr`.
* **Tiêu chuẩn**: Đạt mục tiêu coverage ≥ 85% cho các dịch vụ cốt lõi.

### Profile 3: `devops-toolchain-architect`
* **Mục tiêu**: Thiết lập và duy trì `Dockerfile`, `docker-compose.yml`, `.devcontainer`, `CMakePresets.json`, CI scripts.
* **Tiêu chuẩn**: Đảm bảo môi trường build tái lập 100% (Reproducible Builds).

---

## 4. Quy Định Phê Duyệt (Self-Approval Boundaries)
* **TỰ PHÊ DUYỆT**: Toàn bộ thao tác thêm, sửa, xóa, chạy test, build, commit git trong `c:\workspace\ToolController`.
* **CHỈ HỎI KỸ SƯ V**: Khi có thao tác tác động ra ngoài thư mục `c:\workspace\ToolController` (ổ đĩa hệ thống, thư mục người dùng ngoài workspace).


