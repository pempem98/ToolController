#include "brake_service.h"
#include <stddef.h>

bool brake_service_init(brake_service_t *svc, brake_interface_t *hw_brake, uint32_t delay_ms) {
    if (!svc || !hw_brake) return false;
    svc->hw_brake = hw_brake;
    svc->delay_ms = delay_ms;
    svc->state = BRAKE_STATE_ENGAGED;

    if (svc->hw_brake->init) {
        svc->hw_brake->init(svc->hw_brake);
    }
    // Mặc định an toàn: Khóa phanh
    if (svc->hw_brake->engage) {
        svc->hw_brake->engage(svc->hw_brake);
    }
    return true;
}

bool brake_service_release(brake_service_t *svc) {
    if (!svc || !svc->hw_brake) return false;
    if (svc->hw_brake->release) {
        svc->hw_brake->release(svc->hw_brake);
    }
    svc->state = BRAKE_STATE_RELEASED;
    return true;
}

bool brake_service_engage(brake_service_t *svc) {
    if (!svc || !svc->hw_brake) return false;
    if (svc->hw_brake->engage) {
        svc->hw_brake->engage(svc->hw_brake);
    }
    svc->state = BRAKE_STATE_ENGAGED;
    return true;
}

brake_state_t brake_service_get_state(const brake_service_t *svc) {
    if (!svc) return BRAKE_STATE_ENGAGED;
    return svc->state;
}

