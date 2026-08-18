#ifndef FIELDBUS_ROUTER_H
#define FIELDBUS_ROUTER_H

#include <stdint.h>
#include <stdbool.h>
#include "can_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FIELDBUS_TYPE_CANFD,
    FIELDBUS_TYPE_ETHERCAT
} fieldbus_type_t;

typedef void (*fieldbus_frame_handler_t)(const can_frame_t *frame, void *user_data);

typedef struct {
    fieldbus_type_t type;
    can_interface_t *can_dev;
    fieldbus_frame_handler_t handler;
    void *user_data;
} fieldbus_router_t;

bool fieldbus_router_init(fieldbus_router_t *router, fieldbus_type_t type, can_interface_t *can_dev);
bool fieldbus_router_register_handler(fieldbus_router_t *router, fieldbus_frame_handler_t handler, void *user_data);
bool fieldbus_router_process(fieldbus_router_t *router);

#ifdef __cplusplus
}
#endif

#endif // FIELDBUS_ROUTER_H
