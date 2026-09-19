#include "motor_service.h"
#include <stddef.h>

bool motor_service_init(motor_service_t *svc, motor_instance_t *motor_array, uint8_t count) {
    if (!svc || !motor_array || count == 0) return false;

    svc->motors = motor_array;
    svc->motor_count = count;

    for (uint8_t i = 0; i < count; i++) {
        svc->motors[i].node_id = i + 1;
        svc->motors[i].driver = NULL;
        svc->motors[i].target_position = 0;
        svc->motors[i].actual_position = 0;
        svc->motors[i].encoder_ticks = 0;
        svc->motors[i].direction = MOTOR_DIR_CW;
        svc->motors[i].enabled = false;
    }
    return true;
}

bool motor_service_bind_driver(motor_service_t *svc, uint8_t motor_idx, motor_interface_t *driver) {
    if (!svc || motor_idx >= svc->motor_count || !driver) return false;
    svc->motors[motor_idx].driver = driver;
    if (driver->init) {
        driver->init(driver);
    }
    return true;
}

bool motor_service_enable_all(motor_service_t *svc) {
    if (!svc || !svc->motors) return false;
    for (uint8_t i = 0; i < svc->motor_count; i++) {
        motor_service_enable_motor(svc, i);
    }
    return true;
}

bool motor_service_disable_all(motor_service_t *svc) {
    if (!svc || !svc->motors) return false;
    for (uint8_t i = 0; i < svc->motor_count; i++) {
        motor_service_disable_motor(svc, i);
    }
    return true;
}

bool motor_service_enable_motor(motor_service_t *svc, uint8_t motor_idx) {
    if (!svc || !svc->motors || motor_idx >= svc->motor_count) return false;
    motor_instance_t *m = &svc->motors[motor_idx];
    m->enabled = true;
    if (m->driver && m->driver->set_enabled) {
        return (m->driver->set_enabled(m->driver, true) == STATUS_OK);
    }
    return true;
}

bool motor_service_disable_motor(motor_service_t *svc, uint8_t motor_idx) {
    if (!svc || !svc->motors || motor_idx >= svc->motor_count) return false;
    motor_instance_t *m = &svc->motors[motor_idx];
    m->enabled = false;
    if (m->driver && m->driver->set_enabled) {
        return (m->driver->set_enabled(m->driver, false) == STATUS_OK);
    }
    return true;
}

bool motor_service_set_direction(motor_service_t *svc, uint8_t motor_idx, motor_direction_t dir) {
    if (!svc || !svc->motors || motor_idx >= svc->motor_count) return false;
    motor_instance_t *m = &svc->motors[motor_idx];
    m->direction = dir;
    if (m->driver && m->driver->set_direction) {
        return (m->driver->set_direction(m->driver, dir) == STATUS_OK);
    }
    return true;
}

bool motor_service_move_to(motor_service_t *svc, uint8_t motor_idx, int32_t target_pos, uint32_t speed, motor_direction_t dir) {
    if (!svc || !svc->motors || motor_idx >= svc->motor_count) return false;
    motor_instance_t *m = &svc->motors[motor_idx];
    m->target_position = target_pos;
    m->direction = dir;
    if (m->driver && m->driver->move_to) {
        return (m->driver->move_to(m->driver, target_pos, speed, dir) == STATUS_OK);
    }
    return true;
}

bool motor_service_rotate(motor_service_t *svc, uint8_t motor_idx, motor_direction_t dir, uint32_t speed) {
    if (!svc || !svc->motors || motor_idx >= svc->motor_count) return false;
    motor_instance_t *m = &svc->motors[motor_idx];
    m->direction = dir;
    if (m->driver && m->driver->rotate) {
        return (m->driver->rotate(m->driver, dir, speed) == STATUS_OK);
    }
    return true;
}

bool motor_service_get_actual_position(const motor_service_t *svc, uint8_t motor_idx, int32_t *pos) {
    if (!svc || !svc->motors || !pos || motor_idx >= svc->motor_count) return false;
    const motor_instance_t *m = &svc->motors[motor_idx];
    if (m->driver && m->driver->get_actual_position) {
        return (m->driver->get_actual_position(m->driver, pos) == STATUS_OK);
    }
    *pos = m->actual_position;
    return true;
}

bool motor_service_get_encoder(const motor_service_t *svc, uint8_t motor_idx, int32_t *ticks) {
    if (!svc || !svc->motors || !ticks || motor_idx >= svc->motor_count) return false;
    const motor_instance_t *m = &svc->motors[motor_idx];
    if (m->driver && m->driver->get_encoder_ticks) {
        return (m->driver->get_encoder_ticks(m->driver, ticks) == STATUS_OK);
    }
    *ticks = m->encoder_ticks;
    return true;
}

bool motor_service_stop(motor_service_t *svc, uint8_t motor_idx) {
    if (!svc || !svc->motors || motor_idx >= svc->motor_count) return false;
    motor_instance_t *m = &svc->motors[motor_idx];
    if (m->driver && m->driver->stop) {
        return (m->driver->stop(m->driver) == STATUS_OK);
    }
    return true;
}

bool motor_service_stop_all(motor_service_t *svc) {
    if (!svc || !svc->motors) return false;
    bool all_ok = true;
    for (uint8_t i = 0; i < svc->motor_count; i++) {
        if (!motor_service_stop(svc, i)) {
            all_ok = false;
        }
    }
    return all_ok;
}

bool motor_service_home(motor_service_t *svc, uint8_t motor_idx) {
    if (!svc || !svc->motors || motor_idx >= svc->motor_count) return false;
    motor_instance_t *m = &svc->motors[motor_idx];
    if (m->driver && m->driver->home) {
        return (m->driver->home(m->driver) == STATUS_OK);
    }
    return true;
}

bool motor_service_rotate_all(motor_service_t *svc, const motor_direction_t *dirs, const uint32_t *speeds) {
    if (!svc || !svc->motors || !dirs || !speeds) return false;
    bool all_ok = true;
    for (uint8_t i = 0; i < svc->motor_count; i++) {
        if (!motor_service_rotate(svc, i, dirs[i], speeds[i])) {
            all_ok = false;
        }
    }
    return all_ok;
}

bool motor_service_move_all(motor_service_t *svc, const int32_t *target_positions, const uint32_t *speeds, const motor_direction_t *dirs) {
    if (!svc || !svc->motors || !target_positions || !speeds || !dirs) return false;
    bool all_ok = true;
    for (uint8_t i = 0; i < svc->motor_count; i++) {
        if (!motor_service_move_to(svc, i, target_positions[i], speeds[i], dirs[i])) {
            all_ok = false;
        }
    }
    return all_ok;
}

bool motor_service_update(motor_service_t *svc) {
    if (!svc || !svc->motors) return false;

    for (uint8_t i = 0; i < svc->motor_count; i++) {
        motor_instance_t *m = &svc->motors[i];
        if (m->driver) {
            if (m->driver->get_actual_position) {
                m->driver->get_actual_position(m->driver, &m->actual_position);
            }
            if (m->driver->get_encoder_ticks) {
                m->driver->get_encoder_ticks(m->driver, &m->encoder_ticks);
            }
        }
    }
    return true;
}

