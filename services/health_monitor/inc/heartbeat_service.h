#ifndef HEARTBEAT_SERVICE_H
#define HEARTBEAT_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t last_heartbeat_timestamp;
    uint32_t timeout_threshold_ms;
    uint32_t heartbeat_counter;
} heartbeat_service_t;

void heartbeat_init(heartbeat_service_t *hb, uint32_t timeout_ms);
void heartbeat_rx_signal(heartbeat_service_t *hb, uint32_t current_time_ms);
bool heartbeat_is_alive(const heartbeat_service_t *hb, uint32_t current_time_ms);

#ifdef __cplusplus
}
#endif

#endif // HEARTBEAT_SERVICE_H
