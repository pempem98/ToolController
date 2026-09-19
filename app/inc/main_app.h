#ifndef MAIN_APP_H
#define MAIN_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Khởi tạo toàn bộ Application & Domain Services (gọi trước khi start RTOS kernel).
 * 
 * @details Thiết lập trạng thái hệ thống, khởi tạo các dịch vụ nghiệp vụ (Motor, Brake,
 *          Operator, CLI) và tạo các RTOS Task trên FreeRTOS.
 * 
 * @return void
 * @note Được gọi tuần tự trong main.c trước khi kích hoạt osKernelStart().
 */
void App_Init(void);

/**
 * @brief Khởi động vòng lặp thực thi của hệ thống (dùng cho môi trường Bare-metal / HOST).
 * 
 * @details Khi chạy với FreeRTOS trên target MCU, kernel scheduler được khởi động bởi
 *          osKernelStart() tại main.c. Hàm này dùng cho môi trường Host Simulation hoặc Bare-metal.
 * 
 * @return void
 */
void App_Start(void);

/**
 * @brief Entry point truyền thống cấp ứng dụng.
 * 
 * @details Chuỗi thực thi tuần tự gọi App_Init() và sau đó gọi App_Start().
 * 
 * @return void
 */
void App_Main(void);

#ifdef __cplusplus
}
#endif

#endif // MAIN_APP_H
