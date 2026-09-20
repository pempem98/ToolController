#ifndef FREERTOS_OSAL_H
#define FREERTOS_OSAL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* osal_task_t;
typedef void* osal_mutex_t;
typedef void* osal_queue_t;
typedef void* osal_timer_t;

typedef void (*osal_task_func_t)(void *arg);
typedef void (*osal_timer_cb_t)(void *arg);

// Task management
osal_task_t osal_task_create(const char *name, osal_task_func_t entry, void *arg, uint32_t stack_size, uint8_t priority);
void osal_task_delete(osal_task_t task);

// Mutex management
osal_mutex_t osal_mutex_create(void);
bool osal_mutex_lock(osal_mutex_t mtx, uint32_t timeout_ms);
bool osal_mutex_unlock(osal_mutex_t mtx);

// Queue management
osal_queue_t osal_queue_create(uint32_t depth, uint32_t item_size);
bool osal_queue_send(osal_queue_t q, const void *item, uint32_t timeout_ms);
bool osal_queue_receive(osal_queue_t q, void *item, uint32_t timeout_ms);

// Software Timer
osal_timer_t osal_timer_create(const char *name, uint32_t period_ms, bool auto_reload, osal_timer_cb_t cb, void *arg);
bool osal_timer_start(osal_timer_t tmr);
bool osal_timer_stop(osal_timer_t tmr);

// Time Utilities
void osal_delay_ms(uint32_t ms);
uint32_t osal_get_tick_ms(void);

/**
 * @brief Vào vùng tới hạn (critical section) để bảo vệ truy cập nguyên tử tới biến/cờ
 *        được chia sẻ giữa nhiều task (VD: cờ trạng thái an toàn dùng bởi nhiều RTOS task).
 *
 * @details Trên FreeRTOS, bọc `taskENTER_CRITICAL()` (tắt ngắt tối thiểu, không dùng cho
 *          các đoạn code dài vì sẽ trễ ngắt hệ thống). Trên Host (không FreeRTOS), là no-op
 *          vì test SIL chạy đơn luồng.
 *
 * @warning Luôn phải gọi osal_exit_critical() ngay sau, đoạn code ở giữa phải cực ngắn
 *          (chỉ đọc/ghi 1 biến), không được gọi hàm block hoặc delay bên trong.
 */
void osal_enter_critical(void);

/**
 * @brief Thoát vùng tới hạn đã vào bằng osal_enter_critical().
 */
void osal_exit_critical(void);

#ifdef __cplusplus
}
#endif

#endif // FREERTOS_OSAL_H
