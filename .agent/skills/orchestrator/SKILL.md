---
name: orchestrator
description: >-
  Multi-Agent Orchestrator role for the Surgical Instrument Controller project.
  Use when Kỹ sư V gives a task large enough to span multiple domains (firmware +
  test + toolchain + docs) or that benefits from parallel execution with independent
  cross-verification. Activates the 1 Orchestrator / max-4-Worker / max-2-Verifier
  model with hard concurrency limits. Do NOT use for single-file, single-domain tasks —
  handle those directly without spawning subagents.
---

# Orchestrator Skill — Điều Phối Đa Agent Có Giới Hạn & Kiểm Tra Chéo

Tri thức nền chi tiết nằm tại [`../../rules/orchestration_protocol.md`](../../rules/orchestration_protocol.md) (giao thức đầy đủ) và [`../../rules/platform_adapters.md`](../../rules/platform_adapters.md) (ánh xạ Claude/Gemini/ChatGPT). Skill này là **quy trình vận hành tóm tắt** để áp dụng nhanh, không lặp lại toàn văn 2 file trên.

## Khi Nào Kích Hoạt Skill Này

Kích hoạt khi **tất cả** đúng:
1. Yêu cầu đụng ≥ 2 lớp kiến trúc khác nhau (VD: sửa driver + viết test + cập nhật CI), HOẶC việc đủ lớn để chạy song song có lợi hơn tuần tự.
2. Việc không phải là sửa lỗi 1 dòng / 1 file đơn giản.

Nếu không thoả, **không kích hoạt** — cứ làm trực tiếp, tránh phí token cho việc điều phối không cần thiết (chính là nguyên tắc chống-spam ở mức nhỏ nhất).

## Quy Trình Rút Gọn (5 Bước — Chi Tiết Xem Rule)

1. **Phân rã**: Liệt kê tối đa 4 gói việc, map vào đúng Role Registry (`firmware-worker`, `qa-worker`, `toolchain-worker`, `docs-worker`, `debug-worker`, `solution-worker`).
2. **Giao việc**: Mỗi gói việc có Task Contract 4 phần (phạm vi file, Definition of Done, ràng buộc chuẩn, định dạng báo cáo) — xem mẫu ở rule §2, §6.
3. **Thực thi**: Tối đa 4 Worker đồng thời, mỗi Worker chỉ đụng file trong whitelist của mình.
4. **Kiểm tra chéo bắt buộc**: Verifier ≠ Worker đã làm việc đó, tự chạy lại build/test, trả verdict `PASS`/`FAIL`/`NEEDS_REVISION`. Không có Verifier riêng khả dụng → Orchestrator tự đóng vai nhưng phải chạy lại checklist từ đầu, không tái dùng kết luận Worker.
5. **Hợp nhất**: Chỉ merge khi `PASS`. `NEEDS_REVISION` tối đa 2 vòng lặp. `FAIL` an toàn/kiến trúc → dừng ngay, báo Kỹ sư V.

## Đếm Tải — Bắt Buộc Hiển Thị Tường Minh

Trước khi spawn thêm Worker/Verifier, Orchestrator phải tự liệt kê đang có bao nhiêu agent hoạt động, ví dụ:

```
Đang chạy: firmware-worker (1), qa-worker (1) => 2/4 Worker, 0/2 Verifier.
Còn chỗ để spawn thêm: 2 Worker, 2 Verifier.
```

Nếu đã chạm 4 Worker hoặc 2 Verifier, gói việc mới phải **xếp hàng** (chờ 1 slot trống), không spawn vượt ngưỡng dưới bất kỳ lý do gì kể cả khi Kỹ sư V thúc giục — nếu Kỹ sư V thực sự cần vượt ngưỡng, đó là quyết định thay đổi giao thức, phải hỏi rõ trước.

## Checklist Verifier (Rút Gọn — Bản Đầy Đủ Ở Rule §5)

- [ ] Build 0 error/0 warning
- [ ] Test pass do tự Verifier chạy lại (không tin log Worker)
- [ ] Diff đúng phạm vi đã giao
- [ ] Tuân `coding_standards.md`
- [ ] Không phá tầng kiến trúc (`services/` không include HAL, `interfaces/` phẳng)
- [ ] An toàn: nếu đụng brake/motor/emergency path — có test fail-safe, không race condition mới

## Lưu Ý Đa Nền Tảng

Nếu đang chạy trên Gemini CLI hoặc ChatGPT/Codex CLI (không có cơ chế subagent gốc như Claude Code), mô phỏng Worker/Verifier bằng phiên CLI song song riêng biệt + `git worktree`, theo đúng bảng ánh xạ tại `platform_adapters.md` §2. Giới hạn 4+2 vẫn áp dụng y hệt — đếm bằng số phiên/terminal đang mở cho task này, không phải bằng API call.
