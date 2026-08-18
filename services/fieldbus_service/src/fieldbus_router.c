#include "fieldbus_router.h"
#include <stddef.h>

bool fieldbus_router_init(fieldbus_router_t *router, fieldbus_type_t type, can_interface_t *can_dev) {
    if (!router) return false;
    router->type = type;
    router->can_dev = can_dev;
    router->handler = NULL;
    router->user_data = NULL;
    return true;
}

bool fieldbus_router_register_handler(fieldbus_router_t *router, fieldbus_frame_handler_t handler, void *user_data) {
    if (!router) return false;
    router->handler = handler;
    router->user_data = user_data;
    return true;
}

bool fieldbus_router_process(fieldbus_router_t *router) {
    if (!router || !router->can_dev || !router->can_dev->receive) return false;

    can_frame_t rx_frame;
    if (router->can_dev->receive(router->can_dev, &rx_frame)) {
        if (router->handler) {
            router->handler(&rx_frame, router->user_data);
        }
        return true;
    }
    return false;
}
