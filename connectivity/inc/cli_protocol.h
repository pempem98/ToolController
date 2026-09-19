#ifndef CLI_PROTOCOL_H
#define CLI_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "motor_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CLI_CMD_UNKNOWN = 0,
    CLI_CMD_HELP,
    CLI_CMD_STATUS,
    CLI_CMD_STOP_ALL,
    CLI_CMD_BRAKE,
    CLI_CMD_ROTATE_ALL,
    CLI_CMD_MOVE_ALL,
    CLI_CMD_ROTATE,
    CLI_CMD_MOVE,
    CLI_CMD_LOG
} cli_cmd_type_t;

typedef struct {
    cli_cmd_type_t type;
    union {
        bool brake_engage;
        bool log_enable;
        struct {
            uint8_t           axis;
            motor_direction_t dir;
            uint32_t          speed;
        } rotate;
        struct {
            uint8_t           axis;
            int32_t           pos;
            uint32_t          speed;
            motor_direction_t dir;
        } move;
        struct {
            uint32_t          speeds[4];
            motor_direction_t dirs[4];
        } rotate_all;
        struct {
            int32_t           positions[4];
            uint32_t          speed;
        } move_all;
    } args;
} cli_command_t;

/**
 * @brief Phân tích chuỗi ký tự nhận từ UART thành gói lệnh có cấu trúc
 */
bool cli_protocol_parse(const char *str, cli_command_t *out_cmd);

/**
 * @brief Đóng gói chuỗi định dạng telemetry định kỳ
 */
size_t cli_protocol_format_telemetry(float ch0, float ch1, bool brake, int32_t p0, int32_t p1, char *out_buf, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif // CLI_PROTOCOL_H

