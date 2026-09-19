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

/**
 * @brief Cấu trúc cấu hình phần cứng cho CAN / CAN-FD Controller
 */
typedef struct {
    uint8_t  controller_id; /**< CAN1, CAN2, FDCAN1... */
    uint32_t nominal_baud;  /**< Tốc độ baud thông thường (vd: 500000, 1000000) */
    uint32_t data_baud;     /**< Tốc độ baud cho data phase nếu chạy CAN-FD */
    bool     enable_fd;     /**< Bật tính năng CAN-FD */
} can_hw_config_t;

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
