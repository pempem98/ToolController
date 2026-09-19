#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/**
 * @file app_config.h
 * @brief Các tham số cấu hình toàn cục cấp ứng dụng cho Surgical Instrument Controller.
 */

/** @brief Số lượng trục động cơ được quản lý trong hệ thống */
#define MOTOR_COUNT              4

/** @brief Tốc độ baudrate của mạng CAN bus nội bộ điều khiển động cơ */
#define CAN_BAUDRATE             1000000

/** @brief Thời gian timeout của Watchdog hệ thống (ms) */
#define WATCHDOG_TIMEOUT_MS      1000

/** @brief Thời gian timeout kiểm tra nhịp tim (Heartbeat) từ Host (ms) */
#define HOST_HEARTBEAT_TIMEOUT_MS 500

/** @brief Hệ số lọc thông thấp số mũ (Exponential Moving Average) cho cảm biến analog */
#define SENSOR_ALPHA_FILTER      0.2f

#endif // APP_CONFIG_H
