#ifndef GPIO_INTERFACE_H
#define GPIO_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GPIO_STATE_LOW = 0,
    GPIO_STATE_HIGH = 1
} gpio_state_t;

typedef enum {
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_INTERRUPT_RISING,
    GPIO_MODE_INTERRUPT_FALLING
} gpio_mode_t;

typedef void (*gpio_irq_callback_t)(void *arg);

typedef struct gpio_interface {
    bool (*init)(struct gpio_interface *self, gpio_mode_t mode);
    void (*write)(struct gpio_interface *self, gpio_state_t state);
    gpio_state_t (*read)(struct gpio_interface *self);
    void (*toggle)(struct gpio_interface *self);
    bool (*attach_interrupt)(struct gpio_interface *self, gpio_irq_callback_t cb, void *arg);
    void *priv_data;
} gpio_interface_t;

#ifdef __cplusplus
}
#endif

#endif // GPIO_INTERFACE_H
