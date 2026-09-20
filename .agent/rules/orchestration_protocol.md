# GIAO THỨC ĐIỀU PHỐI ĐA AGENT (ORCHESTRATOR PROTOCOL) — ĐA NỀN TẢNG

> Áp dụng cho **mọi** nền tảng AI coding agent chạy trên dự án này: Claude Code, Gemini CLI, ChatGPT/Codex CLI, Cursor, hoặc bất kỳ agent nào đọc được `AGENTS.md`. Xem [`platform_adapters.md`](platform_adapters.md) để biết mỗi nền tảng hiện thực hoá các khái niệm dưới đây bằng cơ chế cụ thể gì (Task tool, sub-agent, function calling nhiều bước,...).

Thay thế và mở rộng [`autonomous_orchestration.md`](autonomous_orchestration.md). File đó vẫn giữ nguyên phần **ma trận phê duyệt** (mục 2). File này bổ sung phần **cơ chế điều phối, giới hạn tải, và kiểm tra chéo** mà bản cũ còn thiếu.

---

## 1. Vấn Đề Cần Giải Quyết

Bản mô tả cũ ("Lead Agent tự động sinh/theo dõi/kill subagent") là một **tuyên bố ý định**, không phải cơ chế. Nó thiếu 3 thứ bắt buộc phải có để chạy an toàn trên một dự án firmware y tế:

1. **Giới hạn tải (Concurrency Limit)** — không có, dễ dẫn tới "spam" subagent, tốn token, khó truy vết ai làm gì.
2. **Kiểm tra chéo thật sự (Cross-Verification)** — bản cũ để chính Lead Agent tự chấm điểm subagent do mình sinh ra. Đây là **self-grading**, không phải kiểm tra độc lập.
3. **Tính khả chuyển giữa nền tảng** — cơ chế "invoke_subagent" là thuật ngữ riêng của một hệ, không tồn tại y hệt ở nền tảng khác.

Giao thức dưới đây định nghĩa lại 3 điểm này bằng luật rõ ràng, agent nào cũng làm theo được vì diễn đạt bằng vai trò + checklist + văn bản, không phụ thuộc API riêng.

---

## 2. Ba Vai Trò Cố Định (Role Model)

Mọi phiên làm việc multi-agent phân vai theo đúng 3 lớp sau, không hơn không kém:

| Vai trò | Số lượng tối đa đồng thời | Trách nhiệm |
| :--- | :---: | :--- |
| **Orchestrator** (Tổng Công Trình Sư) | **1 duy nhất** | Nhận yêu cầu từ Kỹ sư V, phân rã việc, giao việc, thu kết quả, ra quyết định cuối cùng. Không tự viết code nghiệp vụ phức tạp — chỉ điều phối + review. |
| **Worker Subagent** (Chuyên viên thực thi) | **Tối đa 4 đồng thời** | Thực hiện 1 gói việc hẹp, độc lập (xem §3). Trả về kết quả + tự-báo cáo (self-report) nhưng KHÔNG tự phê duyệt việc của chính mình. |
| **Verifier Subagent** (Chuyên viên kiểm tra chéo) | **Tối đa 2 đồng thời**, luôn ≥1 khi có Worker hoàn thành | Kiểm tra độc lập kết quả của Worker khác (không kiểm tra việc do chính mình làm). Trả về verdict: `PASS` / `FAIL` / `NEEDS_REVISION` kèm lý do cụ thể. |

**Giới hạn cứng tổng thể**: Tại một thời điểm, tổng số Worker + Verifier đang hoạt động **không vượt quá 5**. Đây là ngưỡng chống-spam bắt buộc — Orchestrator phải tự đếm và xếp hàng (queue) nếu vượt ngưỡng, không được "xin phép rồi lách".

> **Vì sao 4+2 chứ không phải một con số tùy hứng**: 4 Worker phủ đủ 4 chuyên môn cố định của dự án (Firmware, Test/QA, Toolchain/DevOps, Docs) chạy song song mà không tranh chấp file; 2 Verifier đủ để một Verifier rà kỹ thuật và một Verifier (có thể chính Kỹ sư V hoặc Orchestrator đóng vai) rà an toàn/kiến trúc, mà vẫn giữ tổng tải trong tầm kiểm soát ngữ cảnh của một phiên làm việc thông thường.

---

## 3. Danh Mục Worker Subagent Chuẩn Hóa (Role Registry)

Mỗi Worker khi được giao việc PHẢI được gán đúng 1 trong các vai trò sau — không tạo vai trò tùy tiện ngoài danh sách để tránh chồng chéo phạm vi:

| Role ID | Skill nạp kèm | Phạm vi làm việc | KHÔNG được đụng vào |
| :--- | :--- | :--- | :--- |
| `firmware-worker` | [`firmware-mcu-engineer`](../skills/firmware-mcu-engineer/SKILL.md), [`system-architect`](../skills/system-architect/SKILL.md) | `app/`, `services/`, `platform/`, `middleware/`, `interfaces/`, `connectivity/` | `tests/`, CI, `.vscode/` |
| `qa-worker` | [`embedded-tester`](../skills/embedded-tester/SKILL.md) | `tests/` (viết/sửa test, mock, stub, coverage) | Sửa logic nghiệp vụ trong `services/`/`app/` để "làm test qua được" |
| `toolchain-worker` | [`embedded-toolchain-architect`](../skills/embedded-toolchain-architect/SKILL.md) | `CMakeLists.txt`, `CMakePresets.json`, `docker/`, `.devcontainer/`, `.vscode/`, `.github/workflows/` | Logic nghiệp vụ firmware |
| `docs-worker` | [`codebase-index`](../skills/codebase-index/SKILL.md) | `README.md`, `AGENTS.md`, `.agent/skills/*/SKILL.md`, comment Doxygen | Không tự ý đổi hành vi code khi "chỉnh tài liệu" |
| `debug-worker` | [`firmware-debugger`](../skills/firmware-debugger/SKILL.md) | Chẩn đoán fault/crash/race condition, đề xuất fix tối thiểu | Refactor diện rộng ngoài phạm vi lỗi đang điều tra |
| `solution-worker` | [`solution-architect`](../skills/solution-architect/SKILL.md) | Thiết kế giao thức, bảo mật, OTA, tích hợp hệ thống ngoài | Chi tiết hiện thực driver cấp thấp |

Verifier **không có skill riêng** — Verifier tạm mượn đúng skill của Worker mà nó đang kiểm tra, cộng thêm checklist ở §5, để đánh giá bằng cùng tiêu chuẩn nhưng với vai trò phản biện độc lập.

---

## 4. Quy Trình 5 Bước (Bắt Buộc Theo Thứ Tự)

```
[Bước 1] TIẾP NHẬN & PHÂN RÃ (Orchestrator)
   - Đọc yêu cầu Kỹ sư V, xác định phạm vi ảnh hưởng (module/layer nào).
   - Phân rã thành tối đa 4 gói việc độc lập (map 1-1 với Role Registry §3).
   - Nếu 1 gói việc là đủ (task nhỏ, 1 file) => KHÔNG dùng multi-agent, tự làm trực tiếp.
     Chỉ kích hoạt cơ chế này khi việc thực sự cần chia nhiều chuyên môn / chạy song song.

[Bước 2] GIAO VIỆC CÓ HỢP ĐỒNG RÕ (Orchestrator -> Worker)
   Mỗi lệnh giao việc bắt buộc có đủ 4 phần (Task Contract):
     1. Phạm vi file/module được phép sửa (whitelist đường dẫn).
     2. Tiêu chí hoàn thành (Definition of Done) — cụ thể, kiểm chứng được
        (VD: "build 0 error/0 warning", "test mới PASS", "không đổi API public").
     3. Ràng buộc phải tuân thủ: coding_standards.md / commit_standards.md.
     4. Định dạng báo cáo trả về (xem §6).

[Bước 3] THỰC THI SONG SONG CÓ GIỚI HẠN (Worker)
   - Tối đa 4 Worker chạy đồng thời, mỗi Worker chỉ đụng vào file trong whitelist của mình
     (tránh xung đột ghi đè khi nhiều agent sửa cùng lúc).
   - Worker hoàn thành => tự chạy build/test cục bộ liên quan đến phần việc của mình,
     đính kèm log thật (không tự khai "đã pass" mà không có log).

[Bước 4] KIỂM TRA CHÉO BẮT BUỘC (Verifier) — KHÔNG ĐƯỢC BỎ QUA
   - Nguyên tắc "không tự chấm bài mình": Verifier PHẢI khác Worker đã tạo ra thay đổi đó.
     - Nếu chỉ có 1 agent instance khả dụng (không thể sinh Verifier riêng), Orchestrator
       tự đóng vai Verifier NHƯNG phải chạy lại toàn bộ checklist §5 từ đầu, độc lập,
       không được tái sử dụng kết luận Worker đã đưa ra.
   - Verifier chạy lại build/test độc lập (không tin log Worker đưa, tự verify).
   - Verifier trả về đúng 1 trong 3 verdict: `PASS`, `FAIL`, `NEEDS_REVISION` + lý do.
   - Nếu 2 Worker đụng vào phần liên quan nhau (VD: firmware-worker đổi interface,
     qa-worker viết test theo interface cũ) => Verifier bắt buộc đối chiếu chéo giữa
     2 kết quả, không chỉ xét từng cái riêng lẻ.

[Bước 5] HỢP NHẤT & BÁO CÁO (Orchestrator)
   - Chỉ merge kết quả khi verdict = PASS.
   - `NEEDS_REVISION` => quay lại Bước 2 với đúng Worker cũ, tối đa 2 vòng lặp sửa lỗi
     cho cùng 1 gói việc; vòng thứ 3 vẫn fail => dừng, báo cáo Kỹ sư V kèm lý do
     (không lặp vô hạn, tự nó là một dạng chống-spam).
   - `FAIL` nghiêm trọng (an toàn/kiến trúc) => dừng toàn bộ pipeline, KHÔNG merge phần
     nào, báo cáo ngay cho Kỹ sư V.
   - Báo cáo cuối cùng cho Kỹ sư V: ngắn gọn, có bảng gói-việc/verdict/agent-thực-hiện,
     kèm link file thay đổi.
```

---

## 5. Checklist Kiểm Tra Chéo Chuẩn (Dùng Cho Mọi Verifier)

Verifier áp dụng checklist này bất kể vai trò Worker là gì, cộng thêm tiêu chí riêng theo domain:

- [ ] **Build sạch**: 0 error, 0 warning trên preset liên quan (`stm32h7a3zit6q` và/hoặc `host-tests`).
- [ ] **Test pass thật**: Chạy lại `ctest`, không chỉ đọc log Worker để lại.
- [ ] **Đúng phạm vi**: Diff không đụng file ngoài whitelist đã giao ở Bước 2.
- [ ] **Tuân chuẩn**: Đối chiếu [`coding_standards.md`](coding_standards.md) (naming, MISRA rules, Doxygen).
- [ ] **Không phá vỡ tầng kiến trúc**: `services/` không include header platform/HAL; `interfaces/` vẫn phẳng, thuần C.
- [ ] **An toàn (nếu đụng brake/motor/emergency path)**: Có test cho fail-safe default, không có race condition mới, không có dynamic allocation mới trong runtime loop.
- [ ] **Commit message hợp lệ**: Theo [`commit_standards.md`](commit_standards.md) nếu Worker đã tạo commit.

Verdict `PASS` chỉ được ghi khi tick đủ toàn bộ mục áp dụng được cho gói việc đó.

---

## 6. Định Dạng Báo Cáo Chuẩn (Contract I/O — dùng chung mọi nền tảng)

Để Orchestrator có thể tổng hợp báo cáo từ các agent chạy trên nền tảng khác nhau (Claude/Gemini/ChatGPT) một cách nhất quán, MỌI Worker/Verifier trả kết quả theo khuôn Markdown tối giản sau (không phụ thuộc format proprietary của riêng nền tảng nào):

```markdown
### [ROLE_ID] <tên gói việc ngắn>
- **Phạm vi đã sửa**: <danh sách file:line>
- **Build/Test**: <PASS/FAIL> — <lệnh đã chạy> — <log rút gọn>
- **Ghi chú rủi ro**: <nếu có, nêu rõ; nếu không, ghi "Không có">
- **Verdict** (chỉ Verifier điền): PASS | FAIL | NEEDS_REVISION — <lý do 1-2 câu>
```

---

## 7. Cơ Chế Chống Spam & Vòng Lặp Vô Hạn

1. **Giới hạn số lượng đồng thời**: tối đa 4 Worker + 2 Verifier (§2) — Orchestrator phải đếm bằng danh sách tường minh trong báo cáo tiến trình, không ước lượng.
2. **Giới hạn số vòng sửa lỗi**: tối đa 2 vòng `NEEDS_REVISION` cho cùng 1 gói việc trước khi phải dừng và hỏi Kỹ sư V.
3. **Giới hạn phạm vi mỗi Worker**: một Worker chỉ nhận 1 gói việc tại một thời điểm; không giao việc mới cho Worker đang chạy dở việc cũ.
4. **Không tự nhân bản vai trò**: không được sinh 2 Worker cùng Role ID chạy song song trên cùng phạm vi file (gây race condition khi ghi file, không phải chỉ race condition trong firmware).
5. **Task nhỏ = không cần cơ chế này**: sửa 1 dòng, 1 file, không cần phân rã multi-agent — dùng multi-agent chỉ khi việc thật sự lớn, đa chuyên môn, hoặc lợi ích chạy song song vượt chi phí điều phối.

---

## 8. Khi Nào Dừng Hỏi Kỹ Sư V (Không Đổi So Với Bản Gốc)

Giữ nguyên ma trận phê duyệt tại [`autonomous_orchestration.md`](autonomous_orchestration.md) mục 2.2. Bổ sung thêm 2 trường hợp riêng của cơ chế multi-agent:

6. **Verifier và Worker bất đồng quá 2 vòng** (§7.2) mà không hội tụ được kết luận chung.
7. **Phát hiện xung đột phạm vi** giữa 2 Worker đang chạy song song muốn sửa cùng 1 file — Orchestrator dừng cả hai, hỏi Kỹ sư V ưu tiên bên nào thay vì tự ý chọn.
