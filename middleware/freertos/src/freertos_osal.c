#include "freertos_osal.h"
#include <stddef.h>

#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#endif

// Portability wrapper: when building without FreeRTOS (e.g. host unit tests), provide safe stub implementation

osal_task_t osal_task_create(const char *name, osal_task_func_t entry, void *arg, uint32_t stack_size, uint8_t priority) {
    (void)name; (void)entry; (void)arg; (void)stack_size; (void)priority;
#ifdef USE_FREERTOS
    TaskHandle_t handle = NULL;
    if (xTaskCreate((TaskFunction_t)entry, name, (configSTACK_DEPTH_TYPE)stack_size, arg, (UBaseType_t)priority, &handle) == pdPASS) {
        return (osal_task_t)handle;
    }
    return NULL;
#else
    return (osal_task_t)(uintptr_t)1;
#endif
}

void osal_task_delete(osal_task_t task) {
    (void)task;
#ifdef USE_FREERTOS
    vTaskDelete((TaskHandle_t)task);
#endif
}

osal_mutex_t osal_mutex_create(void) {
#ifdef USE_FREERTOS
    SemaphoreHandle_t mtx = xSemaphoreCreateMutex();
    return (osal_mutex_t)mtx;
#else
    return (osal_mutex_t)(uintptr_t)1;
#endif
}

bool osal_mutex_lock(osal_mutex_t mtx, uint32_t timeout_ms) {
    (void)mtx; (void)timeout_ms;
#ifdef USE_FREERTOS
    if (!mtx) return false;
    TickType_t ticks = (timeout_ms == 0xFFFFFFFF) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return (xSemaphoreTake((SemaphoreHandle_t)mtx, ticks) == pdTRUE);
#else
    return (mtx != NULL);
#endif
}

bool osal_mutex_unlock(osal_mutex_t mtx) {
    (void)mtx;
#ifdef USE_FREERTOS
    if (!mtx) return false;
    return (xSemaphoreGive((SemaphoreHandle_t)mtx) == pdTRUE);
#else
    return (mtx != NULL);
#endif
}

#ifndef USE_FREERTOS
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t depth;
    uint32_t item_size;
    uint32_t count;
    uint32_t head;
    uint32_t tail;
    uint8_t *buffer;
} mock_queue_t;
#endif

osal_queue_t osal_queue_create(uint32_t depth, uint32_t item_size) {
    (void)depth; (void)item_size;
#ifdef USE_FREERTOS
    QueueHandle_t q = xQueueCreate((UBaseType_t)depth, (UBaseType_t)item_size);
    return (osal_queue_t)q;
#else
    mock_queue_t *mq = (mock_queue_t*)malloc(sizeof(mock_queue_t));
    if (!mq) return NULL;
    mq->depth = depth;
    mq->item_size = item_size;
    mq->count = 0;
    mq->head = 0;
    mq->tail = 0;
    mq->buffer = (uint8_t*)malloc(depth * item_size);
    return (osal_queue_t)mq;
#endif
}

bool osal_queue_send(osal_queue_t q, const void *item, uint32_t timeout_ms) {
    (void)q; (void)item; (void)timeout_ms;
#ifdef USE_FREERTOS
    if (!q || !item) return false;
    TickType_t ticks = (timeout_ms == 0xFFFFFFFF) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return (xQueueSend((QueueHandle_t)q, item, ticks) == pdTRUE);
#else
    if (!q || !item) return false;
    mock_queue_t *mq = (mock_queue_t*)q;
    if (mq->count >= mq->depth) return false;
    memcpy(&mq->buffer[mq->tail * mq->item_size], item, mq->item_size);
    mq->tail = (mq->tail + 1) % mq->depth;
    mq->count++;
    return true;
#endif
}

bool osal_queue_receive(osal_queue_t q, void *item, uint32_t timeout_ms) {
    (void)q; (void)item; (void)timeout_ms;
#ifdef USE_FREERTOS
    if (!q || !item) return false;
    TickType_t ticks = (timeout_ms == 0xFFFFFFFF) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return (xQueueReceive((QueueHandle_t)q, item, ticks) == pdTRUE);
#else
    if (!q || !item) return false;
    mock_queue_t *mq = (mock_queue_t*)q;
    if (mq->count == 0) return false;
    memcpy(item, &mq->buffer[mq->head * mq->item_size], mq->item_size);
    mq->head = (mq->head + 1) % mq->depth;
    mq->count--;
    return true;
#endif
}

osal_timer_t osal_timer_create(const char *name, uint32_t period_ms, bool auto_reload, osal_timer_cb_t cb, void *arg) {
    (void)name; (void)period_ms; (void)auto_reload; (void)cb; (void)arg;
#ifdef USE_FREERTOS
    TimerHandle_t tmr = xTimerCreate(name, pdMS_TO_TICKS(period_ms), auto_reload ? pdTRUE : pdFALSE, arg, (TimerCallbackFunction_t)cb);
    return (osal_timer_t)tmr;
#else
    return (osal_timer_t)(uintptr_t)1;
#endif
}

bool osal_timer_start(osal_timer_t tmr) {
    (void)tmr;
#ifdef USE_FREERTOS
    if (!tmr) return false;
    return (xTimerStart((TimerHandle_t)tmr, 0) == pdPASS);
#else
    return (tmr != NULL);
#endif
}

bool osal_timer_stop(osal_timer_t tmr) {
    (void)tmr;
#ifdef USE_FREERTOS
    if (!tmr) return false;
    return (xTimerStop((TimerHandle_t)tmr, 0) == pdPASS);
#else
    return (tmr != NULL);
#endif
}

void osal_delay_ms(uint32_t ms) {
    (void)ms;
#ifdef USE_FREERTOS
    vTaskDelay(pdMS_TO_TICKS(ms));
#endif
}

static uint32_t g_host_tick_ms = 0;

uint32_t osal_get_tick_ms(void) {
#ifdef USE_FREERTOS
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
#else
    return g_host_tick_ms++;
#endif
}
