#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t id;
    uint8_t  data[64];
    uint8_t  len;
    bool     is_extended;
    bool     is_fd;
} can_frame_t;

typedef struct can_interface {
    bool (*init)(struct can_interface *self, uint32_t baudrate);
    bool (*send)(struct can_interface *self, const can_frame_t *frame);
    bool (*receive)(struct can_interface *self, can_frame_t *frame);
    bool (*set_filter)(struct can_interface *self, uint32_t id, uint32_t mask);
    void *priv_data;
} can_interface_t;

#ifdef __cplusplus
}
#endif

#endif // CAN_INTERFACE_H
