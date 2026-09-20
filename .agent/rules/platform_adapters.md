# BẢNG ÁNH XẠ NỀN TẢNG (PLATFORM ADAPTERS) — CLAUDE / GEMINI / CHATGPT

> Mục đích: [`orchestration_protocol.md`](orchestration_protocol.md) mô tả các khái niệm (Orchestrator, Worker, Verifier, giới hạn 4+2) một cách trung lập nền tảng. File này chỉ ra **mỗi nền tảng AI hiện thực hoá các khái niệm đó bằng cơ chế cụ thể gì**, để bất kỳ agent nào mở dự án cũng tự ánh xạ được vào công cụ nó đang có.

---

## 1. Nguyên Tắc Chung: `AGENTS.md` Là Điểm Vào Duy Nhất

`AGENTS.md` ở gốc repo là định dạng đang được nhiều công cụ (Claude Code, OpenAI Codex CLI, Cursor, một phần Gemini CLI qua `GEMINI.md`/symlink) tự động đọc khi mở thư mục dự án. Quy ước dự án này:

- **Claude Code**: đọc `AGENTS.md` + toàn bộ `.agent/rules/*.md` + `.agent/skills/*/SKILL.md` (qua cơ chế skill riêng của Claude Code / Antigravity).
- **Gemini CLI**: đọc `GEMINI.md` nếu có, hoặc `AGENTS.md` tùy version. **Khuyến nghị**: giữ 1 file `GEMINI.md` ngắn ở gốc chỉ có 1 dòng trỏ sang `AGENTS.md` để tránh trùng lặp nội dung phải bảo trì 2 nơi (xem mẫu ở §5).
- **ChatGPT / Codex CLI**: đọc `AGENTS.md` theo đúng chuẩn Codex đang dùng (không cần file riêng).
- Khi một nền tảng không tự động đọc `.agent/skills/*/SKILL.md` (vì đó là cơ chế riêng của Claude/Antigravity), agent trên nền tảng đó vẫn PHẢI tự đọc các file này thủ công trước khi đóng vai một Worker — nội dung skill là tri thức domain-specific, không phải cú pháp riêng của Claude.

---

## 2. Ánh Xạ Vai Trò -> Cơ Chế Thực Thi Theo Từng Nền Tảng

| Khái niệm trung lập | Claude Code | Gemini CLI | ChatGPT / Codex CLI |
| :--- | :--- | :--- | :--- |
| **Orchestrator** | Phiên chính (main agent loop) của Claude Code | Phiên chính của `gemini` CLI | Phiên chính của Codex CLI / ChatGPT với quyền chạy shell |
| **Worker Subagent** | `Agent` tool (Task tool), `subagent_type` tùy chuyên môn, chạy `run_in_background` khi song song | Chưa có sub-agent gốc ổn định trên mọi bản: mô phỏng bằng cách **mở phiên `gemini` CLI mới trong terminal/tab riêng** với đúng 1 Task Contract (§2 orchestration_protocol.md) làm prompt đầu vào | Mô phỏng bằng **nhiều phiên Codex CLI song song** (mỗi phiên = 1 terminal/worktree), hoặc dùng tính năng multi-file agent nếu bản đang dùng hỗ trợ |
| **Verifier Subagent** | `Agent` tool với `subagent_type` khác Worker đã dùng (VD: `code-reviewer`, hoặc `general-purpose` với prompt kiểm tra độc lập) | Phiên `gemini` CLI thứ hai, KHÔNG kế thừa context của phiên đã sinh code, chỉ nhận diff + Task Contract để rà độc lập | Phiên Codex CLI thứ hai, chạy `git diff` độc lập rồi áp checklist §5, không đọc lại lịch sử chat của phiên đã viết code |
| **Giới hạn 4 Worker / 2 Verifier đồng thời** | Đếm qua số lời gọi `Agent` đang `run_in_background` chưa nhận notification hoàn thành | Đếm qua số cửa sổ terminal/tab đang mở phiên `gemini` cho task này | Đếm qua số phiên Codex CLI / worktree đang chạy song song cho task này |
| **Task Contract (giao việc)** | Nội dung `prompt` truyền vào `Agent` tool | Prompt đầu tiên gõ vào phiên `gemini` mới | Prompt đầu tiên gõ vào phiên Codex mới, hoặc nội dung issue/PR draft |
| **Báo cáo chuẩn hóa** | Text trả về từ `Agent` tool theo khuôn §6 orchestration_protocol.md | Output cuối phiên `gemini`, dán thủ công vào phiên Orchestrator theo đúng khuôn | Output cuối phiên Codex, dán vào PR description hoặc phiên Orchestrator theo đúng khuôn |
| **Cô lập để tránh xung đột ghi file** | `isolation: "worktree"` của `Agent` tool khi cần | `git worktree add` thủ công cho mỗi phiên `gemini` chạy song song | `git worktree add` thủ công cho mỗi phiên Codex chạy song song |

---

## 3. Vì Sao Không Ép Một Cơ Chế Kỹ Thuật Duy Nhất

Claude Code có sẵn cơ chế subagent chuẩn (Agent tool, giới hạn/theo dõi tự động qua harness). Gemini CLI và Codex CLI tại thời điểm viết tài liệu này **không có API subagent y hệt** — cách khả thi nhất là dùng nhiều phiên CLI song song trên cùng repo, phối hợp bằng file/worktree/git branch, không phải bằng gọi hàm nội bộ. Do đó giao thức ở [`orchestration_protocol.md`](orchestration_protocol.md) được viết ở mức khái niệm (vai trò + hợp đồng + checklist bằng văn bản Markdown) để bất kỳ nền tảng nào cũng tuân theo được, thay vì viết theo cú pháp API của riêng một hãng.

---

## 4. Quy Tắc Khi Nhiều Nền Tảng Cùng Làm Việc Trên Cùng Repo

Nếu Kỹ sư V dùng đồng thời cả Claude Code, Gemini CLI, và ChatGPT/Codex trên cùng dự án (VD: mỗi công cụ phụ trách 1 nhánh git riêng), áp dụng thêm:

1. **Một Orchestrator, một nguồn sự thật**: chỉ 1 phiên (bất kể nền tảng nào) được giữ vai Orchestrator tại một thời điểm cho một yêu cầu. Không để 2 nền tảng cùng tự nhận là Orchestrator và tự phân rã việc — sẽ đụng độ giới hạn 4+2 vì mỗi bên đếm tải riêng, không thấy nhau.
2. **Trạng thái chia sẻ qua git, không qua bộ nhớ nội bộ**: vì mỗi nền tảng có bộ nhớ/context riêng biệt, tiến độ và Task Contract đang treo phải được ghi vào một file theo dõi tạm thời (khuyến nghị: `--fresh` branch hoặc ghi chú trong PR description), không giả định nền tảng khác "nhớ" được gì đã giao trước đó.
3. **Verifier khác nền tảng càng tốt**: nếu Worker (VD: Claude subagent) viết code, ưu tiên để Verifier là phiên trên nền tảng khác (VD: Gemini CLI chạy `git diff` rồi áp checklist) — giảm rủi ro cùng một "thiên kiến mô hình" bỏ sót cùng loại lỗi ở cả 2 bước.
4. **Chuẩn commit/coding style là bất biến**: [`coding_standards.md`](coding_standards.md) và [`commit_standards.md`](commit_standards.md) áp dụng như nhau bất kể agent nào tạo ra thay đổi — không có "phiên bản riêng" theo từng nền tảng.

---

## 5. Mẫu File Trỏ Cho Gemini CLI (Nếu Cần Tạo)

Nếu môi trường Gemini CLI đang dùng không tự đọc `AGENTS.md`, tạo `GEMINI.md` ở gốc repo với nội dung tối giản trỏ sang, KHÔNG sao chép lại nội dung (tránh lệch đồng bộ giữa 2 file khi cập nhật):

```markdown
# GEMINI.md
Toàn bộ hướng dẫn vận hành, persona, coding standards và giao thức điều phối multi-agent
của dự án này nằm tại `AGENTS.md` và thư mục `.agent/`. Đọc `AGENTS.md` trước khi bắt đầu.
```
