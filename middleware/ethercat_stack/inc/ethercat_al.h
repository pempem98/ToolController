#ifndef ETHERCAT_AL_H
#define ETHERCAT_AL_H

#include "ethercat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ecat_al_state_t current_state;
    ecat_al_state_t target_state;
    uint16_t        al_status_code;
    bool            state_change_pending;
} ethercat_al_t;

bool ethercat_al_init(ethercat_al_t *al);
bool ethercat_al_request_state(ethercat_al_t *al, ecat_al_state_t requested_state);
ecat_al_state_t ethercat_al_get_state(const ethercat_al_t *al);
bool ethercat_al_update(ethercat_al_t *al);

#ifdef __cplusplus
}
#endif

#endif // ETHERCAT_AL_H
