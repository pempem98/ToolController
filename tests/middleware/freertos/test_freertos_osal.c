#include "unity.h"
#include "freertos_osal.h"
#include <stdint.h>

void test_freertos_osal_setUp(void) {}
void test_freertos_osal_tearDown(void) {}

void test_osal_task_create_and_delete(void) {
    osal_task_t task = osal_task_create("test_task", NULL, NULL, 128, 1);
    TEST_ASSERT_NOT_NULL(task);
    osal_task_delete(task);
}

void test_osal_mutex_lock_unlock(void) {
    osal_mutex_t mtx = osal_mutex_create();
    TEST_ASSERT_NOT_NULL(mtx);
    TEST_ASSERT_TRUE(osal_mutex_lock(mtx, 100));
    TEST_ASSERT_TRUE(osal_mutex_unlock(mtx));
}

void test_osal_queue_send_receive(void) {
    osal_queue_t q = osal_queue_create(5, sizeof(uint32_t));
    TEST_ASSERT_NOT_NULL(q);

    uint32_t val_tx = 0xDEADBEEF;
    uint32_t val_rx = 0;

    TEST_ASSERT_TRUE(osal_queue_send(q, &val_tx, 100));
    TEST_ASSERT_TRUE(osal_queue_receive(q, &val_rx, 100));
    TEST_ASSERT_EQUAL_HEX32(val_tx, val_rx);
}

void test_osal_timer_start_stop(void) {
    osal_timer_t tmr = osal_timer_create("test_timer", 100, true, NULL, NULL);
    TEST_ASSERT_NOT_NULL(tmr);
    TEST_ASSERT_TRUE(osal_timer_start(tmr));
    TEST_ASSERT_TRUE(osal_timer_stop(tmr));
}

void test_osal_time_utilities(void) {
    uint32_t t1 = osal_get_tick_ms();
    osal_delay_ms(10);
    uint32_t t2 = osal_get_tick_ms();
    TEST_ASSERT_TRUE(t2 >= t1);
}


