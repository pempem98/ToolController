#ifndef OS_INTERFACE_H
#define OS_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* os_task_handle_t;
typedef void* os_queue_handle_t;
typedef void* os_mutex_handle_t;
typedef void* os_semaphore_handle_t;

typedef void (*os_task_func_t)(void *arg);

typedef struct {
    uint32_t current_tick_ms;
} os_system_info_t;

#ifdef __cplusplus
}
#endif

#endif // OS_INTERFACE_H
