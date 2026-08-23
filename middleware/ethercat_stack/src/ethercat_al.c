#include "ethercat_al.h"

bool ethercat_al_init(ethercat_al_t *al) {
    if (!al) return false;
    al->current_state = ECAT_AL_STATE_INIT;
    al->target_state = ECAT_AL_STATE_INIT;
    al->al_status_code = 0x0000;
    al->state_change_pending = false;
    return true;
}

bool ethercat_al_request_state(ethercat_al_t *al, ecat_al_state_t requested_state) {
    if (!al) return false;

    // Validate valid AL state transitions according to ETG.1000
    switch (requested_state) {
        case ECAT_AL_STATE_INIT:
        case ECAT_AL_STATE_PRE_OP:
        case ECAT_AL_STATE_SAFE_OP:
        case ECAT_AL_STATE_OP:
        case ECAT_AL_STATE_BOOTSTRAP:
            al->target_state = requested_state;
            al->state_change_pending = true;
            return true;
        default:
            al->al_status_code = 0x0011; // Invalid requested state
            return false;
    }
}

ecat_al_state_t ethercat_al_get_state(const ethercat_al_t *al) {
    if (!al) return ECAT_AL_STATE_NONE;
    return al->current_state;
}

bool ethercat_al_update(ethercat_al_t *al) {
    if (!al || !al->state_change_pending) return false;

    // Perform state transition logic
    al->current_state = al->target_state;
    al->state_change_pending = false;
    al->al_status_code = 0x0000;
    return true;
}
