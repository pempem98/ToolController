#ifndef TMC2209_DRIVER_H
#define TMC2209_DRIVER_H

#include "motor_interface.h"
#include "uart_interface.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t            axis_id;
    GPIO_TypeDef       *step_port;
    uint16_t           step_pin;
    GPIO_TypeDef       *dir_port;
    uint16_t           dir_pin;
    GPIO_TypeDef       *en_port;
    uint16_t           en_pin;
    uart_interface_t   *uart;
    bool               dir_inverted;
    motor_direction_t  current_dir;
    int32_t            current_steps;
    int32_t            target_steps;
    bool               enabled;
} tmc2209_motor_priv_t;

/**
 * @brief Khởi tạo adapter động cơ bước TMC2209 cho một trục
 */
void tmc2209_driver_create(motor_interface_t *adapter, 
                           tmc2209_motor_priv_t *priv,
                           uint8_t axis_id,
                           GPIO_TypeDef *step_port, uint16_t step_pin,
                           GPIO_TypeDef *dir_port, uint16_t dir_pin,
                           GPIO_TypeDef *en_port, uint16_t en_pin,
                           uart_interface_t *uart,
                           bool dir_inverted);

#ifdef __cplusplus
}
#endif

#endif // TMC2209_DRIVER_H

