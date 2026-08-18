#include "heartbeat_service.h"

void heartbeat_init(heartbeat_service_t *hb, uint32_t timeout_ms) {
    if (!hb) return;
    hb->last_heartbeat_timestamp = 0;
    hb->timeout_threshold_ms = timeout_ms;
    hb->heartbeat_counter = 0;
}

void heartbeat_rx_signal(heartbeat_service_t *hb, uint32_t current_time_ms) {
    if (!hb) return;
    hb->last_heartbeat_timestamp = current_time_ms;
    hb->heartbeat_counter++;
}

bool heartbeat_is_alive(const heartbeat_service_t *hb, uint32_t current_time_ms) {
    if (!hb) return false;
    return (current_time_ms - hb->last_heartbeat_timestamp) <= hb->timeout_threshold_ms;
}
