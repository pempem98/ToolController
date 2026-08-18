#ifndef OS_INTERFACE_H
#define OS_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* os_mutex_t;
typedef void* os_queue_t;
typedef void* os_task_t;

void os_delay_ms(uint32_t ms);
uint32_t os_get_tick_ms(void);

os_mutex_t os_mutex_create(void);
bool os_mutex_lock(os_mutex_t mutex, uint32_t timeout_ms);
bool os_mutex_unlock(os_mutex_t mutex);

os_queue_t os_queue_create(uint32_t item_count, uint32_t item_size);
bool os_queue_send(os_queue_t queue, const void *item, uint32_t timeout_ms);
bool os_queue_receive(os_queue_t queue, void *item, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif // OS_INTERFACE_H
