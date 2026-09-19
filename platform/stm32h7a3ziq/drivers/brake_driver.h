#ifndef BRAKE_DRIVER_H
#define BRAKE_DRIVER_H

#include "brake_interface.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
    bool          is_currently_engaged;
} stm32_brake_priv_t;

void stm32_brake_driver_create(brake_interface_t *adapter,
                              stm32_brake_priv_t *priv,
                              GPIO_TypeDef *port,
                              uint16_t pin);

#ifdef __cplusplus
}
#endif

#endif // BRAKE_DRIVER_H

