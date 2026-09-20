#ifndef CLI_SERVICE_H
#define CLI_SERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "console_interface.h"
#include "motor_service.h"
#include "operator_service.h"
#include "brake_service.h"
#include "cli_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLI_SERVICE_RX_BUFFER_SIZE 128

#ifndef CLI_TELEMETRY_DEFAULT_ENABLED
#define CLI_TELEMETRY_DEFAULT_ENABLED 0
#endif

typedef struct {
    console_interface_t *console;
    motor_service_t     *motor_svc;
    operator_service_t  *op_svc;
    brake_service_t     *brake_svc;
    char                 rx_buffer[CLI_SERVICE_RX_BUFFER_SIZE];
    uint8_t              rx_index;
    uint32_t             last_telemetry_tick;
    uint32_t             telemetry_interval_ms;
    bool                 telemetry_enabled;
} cli_service_t;

/**
 * @brief Khởi tạo CLI service quản lý logging telemetry và nhận lệnh điều khiển
 */
bool cli_service_init(cli_service_t *svc,
                      console_interface_t *console,
                      motor_service_t *motor_svc,
                      operator_service_t *op_svc,
                      brake_service_t *brake_svc);

/**
 * @brief Xử lý định kỳ: in telemetry log và đọc/thực thi command từ cổng serial
 */
void cli_service_process(cli_service_t *svc, uint32_t current_tick_ms);

/**
 * @brief Bật/tắt in log telemetry tự động
 */
void cli_service_set_telemetry_enabled(cli_service_t *svc, bool enabled);

#ifdef __cplusplus
}
#endif

#endif // CLI_SERVICE_H

