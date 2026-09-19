#ifndef BRAKE_SERVICE_H
#define BRAKE_SERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "brake_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BRAKE_STATE_ENGAGED,
    BRAKE_STATE_RELEASED
} brake_state_t;

typedef struct {
    brake_interface_t *hw_brake;
    uint32_t           delay_ms;
    brake_state_t      state;
} brake_service_t;

/**
 * @brief Khởi tạo dịch vụ kiểm soát phanh an toàn
 */
bool brake_service_init(brake_service_t *svc, brake_interface_t *hw_brake, uint32_t delay_ms);

/**
 * @brief Mở phanh an toàn
 */
bool brake_service_release(brake_service_t *svc);

/**
 * @brief Khóa phanh an toàn
 */
bool brake_service_engage(brake_service_t *svc);

/**
 * @brief Lấy trạng thái hiện tại của phanh
 */
brake_state_t brake_service_get_state(const brake_service_t *svc);

#ifdef __cplusplus
}
#endif

#endif // BRAKE_SERVICE_H

