#include "watchdog_manager.h"
#include <stddef.h>

bool watchdog_manager_init(watchdog_manager_t *mgr, wdt_interface_t *hw_wdt) {
    if (!mgr || !hw_wdt) return false;

    mgr->hw_wdt = hw_wdt;
    mgr->task_count = 0;

    for (int i = 0; i < MAX_WATCHDOG_TASKS; i++) {
        mgr->last_checkin[i] = 0;
        mgr->timeout_ms[i] = 0;
        mgr->registered[i] = false;
    }

    return mgr->hw_wdt->init(mgr->hw_wdt, 1000);
}

int8_t watchdog_register_task(watchdog_manager_t *mgr, uint32_t timeout_ms) {
    if (!mgr || mgr->task_count >= MAX_WATCHDOG_TASKS) return -1;

    for (int i = 0; i < MAX_WATCHDOG_TASKS; i++) {
        if (!mgr->registered[i]) {
            mgr->registered[i] = true;
            mgr->timeout_ms[i] = timeout_ms;
            mgr->last_checkin[i] = 0;
            mgr->task_count++;
            return (int8_t)i;
        }
    }
    return -1;
}

void watchdog_task_checkin(watchdog_manager_t *mgr, int8_t task_id, uint32_t current_time_ms) {
    if (!mgr || task_id < 0 || task_id >= MAX_WATCHDOG_TASKS || !mgr->registered[task_id]) return;
    mgr->last_checkin[task_id] = current_time_ms;
}

bool watchdog_check_all_alive(watchdog_manager_t *mgr, uint32_t current_time_ms) {
    if (!mgr || !mgr->hw_wdt) return false;

    for (int i = 0; i < MAX_WATCHDOG_TASKS; i++) {
        if (mgr->registered[i]) {
            if ((current_time_ms - mgr->last_checkin[i]) > mgr->timeout_ms[i]) {
                // One task missed checkin, do NOT feed hardware watchdog!
                return false;
            }
        }
    }

    // All tasks healthy, feed hardware watchdog
    mgr->hw_wdt->feed(mgr->hw_wdt);
    return true;
}
