#ifndef SYSTEM_COORDINATOR_H
#define SYSTEM_COORDINATOR_H

#include <stdbool.h>
#include "app_config.h"
#include "motor_manager.h"
#include "watchdog_manager.h"
#include "heartbeat_service.h"
#include "fieldbus_router.h"
#include "brake_controller.h"
#include "sensor_processor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    motor_manager_t motor_mgr;
    motor_instance_t motors[MOTOR_COUNT];
    motor_protocol_adapter_t protocol_adapter;

    watchdog_manager_t wdt_mgr;
    wdt_interface_t hw_wdt;

    heartbeat_service_t heartbeat;

    fieldbus_router_t router;
    can_interface_t can_dev;

    brake_controller_t brake;
    gpio_interface_t brake_gpio;

    sensor_processor_t sensor_proc;
    sensor_interface_t force_sensor;
    gpio_interface_t endstop_gpio;

    int8_t motion_wdt_id;
    int8_t comms_wdt_id;
    int8_t io_wdt_id;
} system_coordinator_t;

extern system_coordinator_t g_sys;

bool system_coordinator_init(system_coordinator_t *sys);
bool system_coordinator_start_tasks(system_coordinator_t *sys);

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_COORDINATOR_H
