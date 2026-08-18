#include "main_app.h"
#include "system_coordinator.h"

// Forward declaration of tasks starter
extern void rtos_start_scheduler(void);

void App_Main(void) {
    // 1. Initialize system coordinator (Hardware + Services)
    system_coordinator_init(&g_sys);

    // 2. Start FreeRTOS Scheduler and Tasks
    rtos_start_scheduler();
}
