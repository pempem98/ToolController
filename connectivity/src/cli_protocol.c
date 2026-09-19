#include "cli_protocol.h"
#include <stdio.h>
#include <string.h>

bool cli_protocol_parse(const char *str, cli_command_t *out_cmd) {
    if (!str || !out_cmd) return false;
    memset(out_cmd, 0, sizeof(cli_command_t));

    if (strcmp(str, "help") == 0) {
        out_cmd->type = CLI_CMD_HELP;
        return true;
    }
    if (strcmp(str, "status") == 0) {
        out_cmd->type = CLI_CMD_STATUS;
        return true;
    }
    if (strcmp(str, "stop_all") == 0) {
        out_cmd->type = CLI_CMD_STOP_ALL;
        return true;
    }
    if (strncmp(str, "brake ", 6) == 0) {
        out_cmd->type = CLI_CMD_BRAKE;
        out_cmd->args.brake_engage = (strcmp(str + 6, "on") == 0);
        return true;
    }
    if (strncmp(str, "log ", 4) == 0) {
        out_cmd->type = CLI_CMD_LOG;
        out_cmd->args.log_enable = (strcmp(str + 4, "on") == 0);
        return true;
    }
    if (strncmp(str, "rotate_all ", 11) == 0) {
        uint32_t s[4] = {0}, d[4] = {0};
        int parsed = sscanf(str + 11, "%lu %lu %lu %lu %lu %lu %lu %lu",
                            &s[0], &d[0], &s[1], &d[1], &s[2], &d[2], &s[3], &d[3]);
        if (parsed >= 4) {
            out_cmd->type = CLI_CMD_ROTATE_ALL;
            for (int i = 0; i < 4; i++) {
                out_cmd->args.rotate_all.speeds[i] = s[i];
                out_cmd->args.rotate_all.dirs[i] = (d[i] > 0) ? MOTOR_DIR_CCW : MOTOR_DIR_CW;
            }
            return true;
        }
    }
    if (strncmp(str, "move_all ", 9) == 0) {
        int32_t p[4] = {0};
        uint32_t speed = 100;
        int parsed = sscanf(str + 9, "%ld %ld %ld %ld %lu", &p[0], &p[1], &p[2], &p[3], &speed);
        if (parsed >= 5) {
            out_cmd->type = CLI_CMD_MOVE_ALL;
            for (int i = 0; i < 4; i++) {
                out_cmd->args.move_all.positions[i] = p[i];
            }
            out_cmd->args.move_all.speed = speed;
            return true;
        }
    }
    if (strncmp(str, "rotate ", 7) == 0) {
        uint32_t axis = 0, dir = 0, speed = 0;
        if (sscanf(str + 7, "%lu %lu %lu", &axis, &dir, &speed) == 3) {
            out_cmd->type = CLI_CMD_ROTATE;
            out_cmd->args.rotate.axis = (uint8_t)axis;
            out_cmd->args.rotate.dir = (dir > 0) ? MOTOR_DIR_CCW : MOTOR_DIR_CW;
            out_cmd->args.rotate.speed = speed;
            return true;
        }
    }
    if (strncmp(str, "move ", 5) == 0) {
        uint32_t axis = 0, speed = 0, dir = 0;
        int32_t pos = 0;
        if (sscanf(str + 5, "%lu %ld %lu %lu", &axis, &pos, &speed, &dir) == 4) {
            out_cmd->type = CLI_CMD_MOVE;
            out_cmd->args.move.axis = (uint8_t)axis;
            out_cmd->args.move.pos = pos;
            out_cmd->args.move.speed = speed;
            out_cmd->args.move.dir = (dir > 0) ? MOTOR_DIR_CCW : MOTOR_DIR_CW;
            return true;
        }
    }

    out_cmd->type = CLI_CMD_UNKNOWN;
    return false;
}

size_t cli_protocol_format_telemetry(float ch0, float ch1, bool brake, int32_t p0, int32_t p1, char *out_buf, size_t max_len) {
    if (!out_buf || max_len == 0) return 0;
    int n = snprintf(out_buf, max_len, "\r\n[TLM] X:%+d%% Y:%+d%% BRK:%s | M0:%ld M1:%ld\r\n> ",
                     (int)(ch0 * 100.0f), (int)(ch1 * 100.0f), brake ? "ON" : "OFF", (long)p0, (long)p1);
    return (n > 0 && (size_t)n < max_len) ? (size_t)n : 0;
}

