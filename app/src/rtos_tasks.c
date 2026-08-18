#include "system_coordinator.h"
#include "rtos_tasks_config.h"

// RTOS Task loops (Stubs representing RTOS task functions)

void MotionTask(void *pvParameters) {
    (void)pvParameters;
    uint32_t tick = 0;
    while (1) {
        tick += 10;
        motor_manager_update(&g_sys.motor_mgr);
        watchdog_task_checkin(&g_sys.wdt_mgr, g_sys.motion_wdt_id, tick);
    }
}

void CommsTask(void *pvParameters) {
    (void)pvParameters;
    uint32_t tick = 0;
    while (1) {
        tick += 20;
        fieldbus_router_process(&g_sys.router);
        watchdog_task_checkin(&g_sys.wdt_mgr, g_sys.comms_wdt_id, tick);
    }
}

void IOTask(void *pvParameters) {
    (void)pvParameters;
    uint32_t tick = 0;
    while (1) {
        tick += 50;
        sensor_processor_update(&g_sys.sensor_proc);
        watchdog_task_checkin(&g_sys.wdt_mgr, g_sys.io_wdt_id, tick);
    }
}

void HealthTask(void *pvParameters) {
    (void)pvParameters;
    uint32_t tick = 0;
    while (1) {
        tick += 100;
        watchdog_check_all_alive(&g_sys.wdt_mgr, tick);
    }
}

void rtos_start_scheduler(void) {
    // In actual target build, xTaskCreate for MotionTask, CommsTask, IOTask, HealthTask
    // and vTaskStartScheduler()
}
