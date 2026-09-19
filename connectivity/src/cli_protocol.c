#include "cli_protocol.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <inttypes.h>
#include <ctype.h>

bool cli_protocol_parse(const char *str, cli_command_t *out_cmd) {
    if (!str || !out_cmd) return false;
    memset(out_cmd, 0, sizeof(cli_command_t));

    // Trim leading whitespace
    while (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n') {
        str++;
    }
    size_t len = strlen(str);
    // Trim trailing whitespace
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || 
                       str[len - 1] == '\r' || str[len - 1] == '\n')) {
        len--;
    }
    if (len == 0 || len >= 128) {
        out_cmd->type = CLI_CMD_UNKNOWN;
        return false;
    }

    char clean[128];
    memcpy(clean, str, len);
    clean[len] = '\0';

    if (strcmp(clean, "help") == 0 || strcmp(clean, "?") == 0) {
        out_cmd->type = CLI_CMD_HELP;
        return true;
    }
    if (strcmp(clean, "status") == 0) {
        out_cmd->type = CLI_CMD_STATUS;
        return true;
    }
    if (strcmp(clean, "stop_all") == 0 || strcmp(clean, "stop all") == 0) {
        out_cmd->type = CLI_CMD_STOP_ALL;
        return true;
    }
    if (strncmp(clean, "brake ", 6) == 0) {
        const char *arg = clean + 6;
        while (*arg == ' ') arg++;
        if (strcmp(arg, "on") == 0) {
            out_cmd->type = CLI_CMD_BRAKE;
            out_cmd->args.brake_engage = true;
            return true;
        }
        if (strcmp(arg, "off") == 0) {
            out_cmd->type = CLI_CMD_BRAKE;
            out_cmd->args.brake_engage = false;
            return true;
        }
        return false;
    }
    if (strncmp(clean, "log ", 4) == 0) {
        const char *arg = clean + 4;
        while (*arg == ' ') arg++;
        if (strcmp(arg, "on") == 0) {
            out_cmd->type = CLI_CMD_LOG;
            out_cmd->args.log_enable = true;
            return true;
        }
        if (strcmp(arg, "off") == 0) {
            out_cmd->type = CLI_CMD_LOG;
            out_cmd->args.log_enable = false;
            return true;
        }
        return false;
    }

    // rotate all <dir0> <spd0> ... or rotate_all <s0> <d0> ...
    const char *rot_all_ptr = NULL;
    if (strncmp(clean, "rotate all ", 11) == 0) {
        rot_all_ptr = clean + 11;
    } else if (strncmp(clean, "rotate_all ", 11) == 0) {
        rot_all_ptr = clean + 11;
    }
    if (rot_all_ptr) {
        char d[4][16] = {{0}};
        uint32_t s[4] = {0};
        int parsed = sscanf(rot_all_ptr, "%15s %" SCNu32 " %15s %" SCNu32 " %15s %" SCNu32 " %15s %" SCNu32,
                            d[0], &s[0], d[1], &s[1], d[2], &s[2], d[3], &s[3]);
        if (parsed == 8) {
            out_cmd->type = CLI_CMD_ROTATE_ALL;
            for (int i = 0; i < 4; i++) {
                out_cmd->args.rotate_all.speeds[i] = s[i];
                if (strcasecmp(d[i], "ccw") == 0 || strcmp(d[i], "1") == 0) {
                    out_cmd->args.rotate_all.dirs[i] = MOTOR_DIR_CCW;
                } else {
                    out_cmd->args.rotate_all.dirs[i] = MOTOR_DIR_CW;
                }
            }
            return true;
        }
        uint32_t num_s[4] = {0}, num_d[4] = {0};
        int parsed_nums = sscanf(rot_all_ptr, "%" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32 " %" SCNu32,
                                 &num_s[0], &num_d[0], &num_s[1], &num_d[1], &num_s[2], &num_d[2], &num_s[3], &num_d[3]);
        if (parsed_nums >= 4) {
            out_cmd->type = CLI_CMD_ROTATE_ALL;
            for (int i = 0; i < 4; i++) {
                out_cmd->args.rotate_all.speeds[i] = num_s[i];
                out_cmd->args.rotate_all.dirs[i] = (num_d[i] > 0) ? MOTOR_DIR_CCW : MOTOR_DIR_CW;
            }
            return true;
        }
    }

    // move all <p0> <p1> <p2> <p3> <spd> or move_all <p0> <p1> <p2> <p3> <spd>
    const char *move_all_ptr = NULL;
    if (strncmp(clean, "move all ", 9) == 0) {
        move_all_ptr = clean + 9;
    } else if (strncmp(clean, "move_all ", 9) == 0) {
        move_all_ptr = clean + 9;
    }
    if (move_all_ptr) {
        int32_t p[4] = {0};
        uint32_t speed = 100;
        int parsed = sscanf(move_all_ptr, "%" SCNd32 " %" SCNd32 " %" SCNd32 " %" SCNd32 " %" SCNu32,
                            &p[0], &p[1], &p[2], &p[3], &speed);
        if (parsed >= 5) {
            out_cmd->type = CLI_CMD_MOVE_ALL;
            for (int i = 0; i < 4; i++) {
                out_cmd->args.move_all.positions[i] = p[i];
            }
            out_cmd->args.move_all.speed = speed;
            return true;
        }
    }

    // rotate <axis> <dir> <speed>
    if (strncmp(clean, "rotate ", 7) == 0) {
        uint32_t axis = 0, speed = 0;
        char dir_str[16] = {0};
        if (sscanf(clean + 7, "%" SCNu32 " %15s %" SCNu32, &axis, dir_str, &speed) == 3) {
            out_cmd->type = CLI_CMD_ROTATE;
            out_cmd->args.rotate.axis = (uint8_t)axis;
            if (strcasecmp(dir_str, "ccw") == 0 || strcmp(dir_str, "1") == 0) {
                out_cmd->args.rotate.dir = MOTOR_DIR_CCW;
            } else {
                out_cmd->args.rotate.dir = MOTOR_DIR_CW;
            }
            out_cmd->args.rotate.speed = speed;
            return true;
        }
    }

    // move <axis> <dir> <pos> <speed> OR move <axis> <pos> <speed> <dir>
    if (strncmp(clean, "move ", 5) == 0) {
        uint32_t axis = 0;
        char tok2[16] = {0};
        if (sscanf(clean + 5, "%" SCNu32 " %15s", &axis, tok2) == 2) {
            if (strcasecmp(tok2, "cw") == 0 || strcasecmp(tok2, "ccw") == 0) {
                int32_t pos = 0;
                uint32_t speed = 0;
                if (sscanf(clean + 5, "%" SCNu32 " %15s %" SCNd32 " %" SCNu32, &axis, tok2, &pos, &speed) == 4) {
                    out_cmd->type = CLI_CMD_MOVE;
                    out_cmd->args.move.axis = (uint8_t)axis;
                    out_cmd->args.move.dir = (strcasecmp(tok2, "ccw") == 0) ? MOTOR_DIR_CCW : MOTOR_DIR_CW;
                    out_cmd->args.move.pos = pos;
                    out_cmd->args.move.speed = speed;
                    return true;
                }
            } else {
                int32_t pos = 0;
                uint32_t speed = 0, dir = 0;
                if (sscanf(clean + 5, "%" SCNu32 " %" SCNd32 " %" SCNu32 " %" SCNu32, &axis, &pos, &speed, &dir) == 4) {
                    out_cmd->type = CLI_CMD_MOVE;
                    out_cmd->args.move.axis = (uint8_t)axis;
                    out_cmd->args.move.pos = pos;
                    out_cmd->args.move.speed = speed;
                    out_cmd->args.move.dir = (dir > 0) ? MOTOR_DIR_CCW : MOTOR_DIR_CW;
                    return true;
                }
            }
        }
    }

    out_cmd->type = CLI_CMD_UNKNOWN;
    return false;
}

size_t cli_protocol_format_telemetry(float ch0, float ch1, bool brake, int32_t p0, int32_t p1, char *out_buf, size_t max_len) {
    if (!out_buf || max_len == 0) return 0;
    int n = snprintf(out_buf, max_len, "\r\n[TLM] X:%+d%% Y:%+d%% BRK=%d | M0:%" PRId32 " M1:%" PRId32 "\r\n> ",
                     (int)(ch0 * 100.0f), (int)(ch1 * 100.0f), brake ? 1 : 0, p0, p1);
    return (n > 0 && (size_t)n < max_len) ? (size_t)n : 0;
}


