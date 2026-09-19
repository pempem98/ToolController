#include "cli_service.h"
#include <stdio.h>
#include <string.h>

static void console_send_str(console_interface_t *c, const char *str) {
    if (!c || !c->write || !str) return;
    c->write(c, (const uint8_t*)str, strlen(str));
}

bool cli_service_init(cli_service_t *svc,
                      console_interface_t *console,
                      motor_service_t *motor_svc,
                      operator_service_t *op_svc,
                      brake_service_t *brake_svc) {
    if (!svc || !console) return false;

    svc->console = console;
    svc->motor_svc = motor_svc;
    svc->op_svc = op_svc;
    svc->brake_svc = brake_svc;
    svc->rx_index = 0;
    svc->last_telemetry_tick = 0;
    svc->telemetry_interval_ms = 500;
    svc->telemetry_enabled = true;

    memset(svc->rx_buffer, 0, sizeof(svc->rx_buffer));

    console_send_str(console, "\r\n=== ToolController Multi-Motor CLI Service Initialized ===\r\n");
    console_send_str(console, "Type 'help' for command list.\r\n> ");
    return true;
}

void cli_service_set_telemetry_enabled(cli_service_t *svc, bool enabled) {
    if (!svc) return;
    svc->telemetry_enabled = enabled;
}

static void execute_command(cli_service_t *svc, const cli_command_t *cmd) {
    char reply[256];
    console_interface_t *c = svc->console;

    switch (cmd->type) {
        case CLI_CMD_HELP:
            console_send_str(c, "\r\nAvailable Commands:\r\n");
            console_send_str(c, "  status                                  - Display system telemetry\r\n");
            console_send_str(c, "  stop_all                                - Emergency stop all motors\r\n");
            console_send_str(c, "  brake <on|off>                          - Engage or release brake\r\n");
            console_send_str(c, "  rotate <axis> <dir(0/1)> <speed>        - Rotate single motor\r\n");
            console_send_str(c, "  move <axis> <pos> <speed> <dir(0/1)>    - Move single motor to pos\r\n");
            console_send_str(c, "  rotate_all <s0> <d0> <s1> <d1> ...      - Rotate all 4 motors simultaneously\r\n");
            console_send_str(c, "  move_all <p0> <p1> <p2> <p3> <speed>    - Move all 4 motors simultaneously\r\n");
            console_send_str(c, "  log <on|off>                            - Toggle periodic telemetry log\r\n");
            break;

        case CLI_CMD_STATUS: {
            float ch0 = 0.0f, ch1 = 0.0f, ch2 = 0.0f, ch3 = 0.0f;
            if (svc->op_svc) {
                ch0 = operator_service_get_axis(svc->op_svc, 0);
                ch1 = operator_service_get_axis(svc->op_svc, 1);
                ch2 = operator_service_get_axis(svc->op_svc, 2);
                ch3 = operator_service_get_axis(svc->op_svc, 3);
            }
            int32_t p0 = 0, p1 = 0, p2 = 0, p3 = 0;
            int32_t e0 = 0, e1 = 0, e2 = 0, e3 = 0;
            if (svc->motor_svc) {
                motor_service_get_actual_position(svc->motor_svc, 0, &p0);
                motor_service_get_actual_position(svc->motor_svc, 1, &p1);
                motor_service_get_actual_position(svc->motor_svc, 2, &p2);
                motor_service_get_actual_position(svc->motor_svc, 3, &p3);
                motor_service_get_encoder(svc->motor_svc, 0, &e0);
                motor_service_get_encoder(svc->motor_svc, 1, &e1);
                motor_service_get_encoder(svc->motor_svc, 2, &e2);
                motor_service_get_encoder(svc->motor_svc, 3, &e3);
            }
            bool brk = (svc->brake_svc && brake_service_get_state(svc->brake_svc) == BRAKE_STATE_ENGAGED);
            snprintf(reply, sizeof(reply),
                     "\r\n[STATUS] BRK:%s | AXES:[%d, %d, %d, %d]x0.01\r\n"
                     "  M0: pos=%ld, enc=%ld | M1: pos=%ld, enc=%ld\r\n"
                     "  M2: pos=%ld, enc=%ld | M3: pos=%ld, enc=%ld\r\n",
                     brk ? "ENGAGED" : "RELEASED",
                     (int)(ch0 * 100), (int)(ch1 * 100), (int)(ch2 * 100), (int)(ch3 * 100),
                     (long)p0, (long)e0, (long)p1, (long)e1, (long)p2, (long)e2, (long)p3, (long)e3);
            console_send_str(c, reply);
            break;
        }

        case CLI_CMD_STOP_ALL:
            if (svc->motor_svc) {
                motor_service_stop_all(svc->motor_svc);
            }
            console_send_str(c, "\r\n[OK] All motors stopped.\r\n");
            break;

        case CLI_CMD_BRAKE:
            if (cmd->args.brake_engage) {
                if (svc->brake_svc) brake_service_engage(svc->brake_svc);
                if (svc->motor_svc) motor_service_stop_all(svc->motor_svc);
                console_send_str(c, "\r\n[OK] Brake engaged, motors stopped.\r\n");
            } else {
                if (svc->brake_svc) brake_service_release(svc->brake_svc);
                console_send_str(c, "\r\n[OK] Brake released.\r\n");
            }
            break;

        case CLI_CMD_LOG:
            svc->telemetry_enabled = cmd->args.log_enable;
            console_send_str(c, svc->telemetry_enabled ? "\r\n[OK] Log ON.\r\n" : "\r\n[OK] Log OFF.\r\n");
            break;

        case CLI_CMD_ROTATE_ALL:
            if (svc->motor_svc) {
                motor_service_rotate_all(svc->motor_svc, cmd->args.rotate_all.dirs, cmd->args.rotate_all.speeds);
                console_send_str(c, "\r\n[OK] Executed rotate_all on 4 motors.\r\n");
            }
            break;

        case CLI_CMD_MOVE_ALL:
            if (svc->motor_svc) {
                uint32_t speeds[4];
                motor_direction_t dirs[4];
                for (int i = 0; i < 4; i++) {
                    speeds[i] = cmd->args.move_all.speed;
                    dirs[i] = (cmd->args.move_all.positions[i] >= 0) ? MOTOR_DIR_CW : MOTOR_DIR_CCW;
                }
                motor_service_move_all(svc->motor_svc, cmd->args.move_all.positions, speeds, dirs);
                console_send_str(c, "\r\n[OK] Executed move_all on 4 motors.\r\n");
            }
            break;

        case CLI_CMD_ROTATE:
            if (svc->motor_svc) {
                motor_service_rotate(svc->motor_svc, cmd->args.rotate.axis, cmd->args.rotate.dir, cmd->args.rotate.speed);
                snprintf(reply, sizeof(reply), "\r\n[OK] Rotating motor %u (spd=%lu)\r\n", cmd->args.rotate.axis, (unsigned long)cmd->args.rotate.speed);
                console_send_str(c, reply);
            }
            break;

        case CLI_CMD_MOVE:
            if (svc->motor_svc) {
                motor_service_move_to(svc->motor_svc, cmd->args.move.axis, cmd->args.move.pos, cmd->args.move.speed, cmd->args.move.dir);
                snprintf(reply, sizeof(reply), "\r\n[OK] Moving motor %u to %ld\r\n", cmd->args.move.axis, (long)cmd->args.move.pos);
                console_send_str(c, reply);
            }
            break;

        default:
            console_send_str(c, "\r\n[ERR] Unknown command. Type 'help'.\r\n");
            break;
    }
    console_send_str(c, "> ");
}

void cli_service_process(cli_service_t *svc, uint32_t current_tick_ms) {
    if (!svc || !svc->console) return;

    // 1. Nhận ký tự từ console và parse bằng cli_protocol
    uint8_t rx_byte = 0;
    size_t rx_len = 0;
    while (svc->console->read &&
           svc->console->read(svc->console, &rx_byte, 1, &rx_len) == STATUS_OK &&
           rx_len > 0) {
        if (rx_byte == '\r' || rx_byte == '\n') {
            if (svc->rx_index > 0) {
                svc->rx_buffer[svc->rx_index] = '\0';
                cli_command_t cmd;
                cli_protocol_parse(svc->rx_buffer, &cmd);
                execute_command(svc, &cmd);
                svc->rx_index = 0;
            }
        } else if (rx_byte == 0x08 || rx_byte == 0x7F) { // Backspace
            if (svc->rx_index > 0) {
                svc->rx_index--;
                console_send_str(svc->console, "\b \b");
            }
        } else if (svc->rx_index < CLI_SERVICE_RX_BUFFER_SIZE - 1) {
            svc->rx_buffer[svc->rx_index++] = (char)rx_byte;
            char echo[2] = {(char)rx_byte, '\0'};
            console_send_str(svc->console, echo);
        }
    }

    // 2. Telemetry định kỳ
    if (svc->telemetry_enabled && (current_tick_ms - svc->last_telemetry_tick >= svc->telemetry_interval_ms)) {
        svc->last_telemetry_tick = current_tick_ms;

        float ch0 = 0.0f, ch1 = 0.0f;
        if (svc->op_svc) {
            ch0 = operator_service_get_axis(svc->op_svc, 0);
            ch1 = operator_service_get_axis(svc->op_svc, 1);
        }
        int32_t p0 = 0, p1 = 0;
        if (svc->motor_svc) {
            motor_service_get_actual_position(svc->motor_svc, 0, &p0);
            motor_service_get_actual_position(svc->motor_svc, 1, &p1);
        }
        bool brk = (svc->brake_svc && brake_service_get_state(svc->brake_svc) == BRAKE_STATE_ENGAGED);

        char tele[128];
        if (cli_protocol_format_telemetry(ch0, ch1, brk, p0, p1, tele, sizeof(tele)) > 0) {
            console_send_str(svc->console, tele);
        }
    }
}
