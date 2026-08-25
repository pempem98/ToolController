#include "system_coordinator.h"
#include "board_adapters.h"

// Forward declaration for protocol adapter
extern void canfd_adapter_create(motor_protocol_adapter_t *adapter);

system_coordinator_t g_sys;

bool system_coordinator_init(system_coordinator_t *sys) {
    if (!sys) return false;

    // 1. Initialize hardware adapters via platform board
    board_wdt_adapter_create(&sys->hw_wdt);
    board_can_adapter_create(&sys->can_dev);
    board_gpio_adapter_create(&sys->brake_gpio);
    board_gpio_adapter_create(&sys->endstop_gpio);
    canfd_adapter_create(&sys->protocol_adapter);

    // 2. Initialize Health Monitor & Task Watchdog
    watchdog_manager_init(&sys->wdt_mgr, &sys->hw_wdt);
    heartbeat_init(&sys->heartbeat, HOST_HEARTBEAT_TIMEOUT_MS);

    sys->motion_wdt_id = watchdog_register_task(&sys->wdt_mgr, 50);
    sys->comms_wdt_id  = watchdog_register_task(&sys->wdt_mgr, 100);
    sys->io_wdt_id     = watchdog_register_task(&sys->wdt_mgr, 200);

    // 3. Initialize Motion Service
    motor_manager_init(&sys->motor_mgr, sys->motors, MOTOR_COUNT, &sys->protocol_adapter);

    // 4. Initialize Fieldbus Router
    fieldbus_router_init(&sys->router, FIELDBUS_TYPE_CANFD, &sys->can_dev);

    // 5. Initialize IO Service
    brake_controller_init(&sys->brake, &sys->brake_gpio, 100);
    sensor_processor_init(&sys->sensor_proc, &sys->force_sensor, &sys->endstop_gpio, SENSOR_ALPHA_FILTER);

    return true;
}
