#include "main_app.h"
#include "system_coordinator.h"
#include <stddef.h>

/**
 * @brief Khởi tạo toàn bộ tầng ứng dụng và các dịch vụ nghiệp vụ của hệ thống.
 * 
 * @details Hàm thực hiện khởi tạo phần cứng thông qua BSP Board, liên kết các driver
 *          động cơ, khởi tạo cấu hình người vận hành, phanh an toàn và CLI service.
 *          Đồng thời đăng ký tạo các RTOS Task với kernel FreeRTOS.
 * 
 * @note Phải được gọi trong main() trước khi FreeRTOS scheduler (osKernelStart) được kích hoạt.
 * @warning Không được gọi bất kỳ hàm block hay delay nào của RTOS bên trong hàm này vì kernel chưa chạy.
 */
void App_Init(void) {
    // 1. Khởi tạo coordinator hệ thống (phần cứng IoHwAb + domain services)
    system_coordinator_init(&g_sys, NULL);

    // 2. Tạo các FreeRTOS tasks chuyên biệt sẵn sàng cho osKernelStart()
    system_coordinator_start_tasks(&g_sys);
}

/**
 * @brief Bắt đầu vòng lặp thực thi của tầng ứng dụng.
 * 
 * @details Trong mô hình FreeRTOS, việc khởi động bộ lập lịch được thực hiện bởi
 *          hàm chuẩn osKernelStart() tại main.c của CubeMX. Hàm này được giữ lại
 *          để tương thích ngược với mô hình Bare-metal hoặc Host SIL simulation.
 * 
 * @note Với FreeRTOS, hàm này không làm gì (No-op).
 */
void App_Start(void) {
    // Với FreeRTOS, scheduler được kích hoạt bởi osKernelStart() trong main.c
}

/**
 * @brief Hàm điểm vào (Entry point) cấp ứng dụng.
 * 
 * @details Đóng vai trò tổng hợp quy trình khởi tạo (App_Init) và khởi chạy (App_Start).
 *          Thích hợp cho các bài kiểm tra tích hợp hoặc môi trường không dùng CubeMX main.
 */
void App_Main(void) {
    App_Init();
    App_Start();
}
