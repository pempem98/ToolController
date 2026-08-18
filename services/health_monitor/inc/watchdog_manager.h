#ifndef WATCHDOG_MANAGER_H
#define WATCHDOG_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "wdt_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_WATCHDOG_TASKS 8

typedef struct {
    wdt_interface_t *hw_wdt;
    uint32_t last_checkin[MAX_WATCHDOG_TASKS];
    uint32_t timeout_ms[MAX_WATCHDOG_TASKS];
    bool registered[MAX_WATCHDOG_TASKS];
    uint8_t task_count;
} watchdog_manager_t;

bool watchdog_manager_init(watchdog_manager_t *mgr, wdt_interface_t *hw_wdt);
int8_t watchdog_register_task(watchdog_manager_t *mgr, uint32_t timeout_ms);
void watchdog_task_checkin(watchdog_manager_t *mgr, int8_t task_id, uint32_t current_time_ms);
bool watchdog_check_all_alive(watchdog_manager_t *mgr, uint32_t current_time_ms);

#ifdef __cplusplus
}
#endif

#endif // WATCHDOG_MANAGER_H
