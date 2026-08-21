#include <stddef.h>
#include "system_coordinator.h"
#include "rtos_tasks_config.h"

// Task Step implementations (non-blocking step for debugging without FreeRTOS)
void MotionTaskStep(void) {
    static uint32_t tick = 0;
    tick += 10;
    motor_manager_update(&g_sys.motor_mgr);
    watchdog_task_checkin(&g_sys.wdt_mgr, g_sys.motion_wdt_id, tick);
}

void CommsTaskStep(void) {
    static uint32_t tick = 0;
    tick += 20;
    fieldbus_router_process(&g_sys.router);
    watchdog_task_checkin(&g_sys.wdt_mgr, g_sys.comms_wdt_id, tick);
}

void IOTaskStep(void) {
    static uint32_t tick = 0;
    tick += 50;
    sensor_processor_update(&g_sys.sensor_proc);
    watchdog_task_checkin(&g_sys.wdt_mgr, g_sys.io_wdt_id, tick);
}

void HealthTaskStep(void) {
    static uint32_t tick = 0;
    tick += 100;
    watchdog_check_all_alive(&g_sys.wdt_mgr, tick);
}

// RTOS Task entry loops
void MotionTask(void *pvParameters) {
    (void)pvParameters;
    while (1) {
        MotionTaskStep();
    }
}

void CommsTask(void *pvParameters) {
    (void)pvParameters;
    while (1) {
        CommsTaskStep();
    }
}

void IOTask(void *pvParameters) {
    (void)pvParameters;
    while (1) {
        IOTaskStep();
    }
}

void HealthTask(void *pvParameters) {
    (void)pvParameters;
    while (1) {
        HealthTaskStep();
    }
}

void rtos_start_scheduler(void) {
    // Run task steps sequentially in a super-loop (when FreeRTOS kernel is inactive)
    while (1) {
        MotionTaskStep();
        CommsTaskStep();
        IOTaskStep();
        HealthTaskStep();
    }
}
