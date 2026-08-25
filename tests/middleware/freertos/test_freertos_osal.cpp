#include <gtest/gtest.h>
#include "freertos_osal.h"

TEST(FreeRTOSOSALTest, TaskCreateAndDelete) {
    osal_task_t task = osal_task_create("test_task", NULL, NULL, 128, 1);
    EXPECT_NE(task, nullptr);
    osal_task_delete(task);
}

TEST(FreeRTOSOSALTest, MutexLockUnlock) {
    osal_mutex_t mtx = osal_mutex_create();
    EXPECT_NE(mtx, nullptr);
    EXPECT_TRUE(osal_mutex_lock(mtx, 100));
    EXPECT_TRUE(osal_mutex_unlock(mtx));
}

TEST(FreeRTOSOSALTest, QueueSendReceive) {
    osal_queue_t q = osal_queue_create(5, sizeof(uint32_t));
    EXPECT_NE(q, nullptr);

    uint32_t val_tx = 0xDEADBEEF;
    uint32_t val_rx = 0;

    EXPECT_TRUE(osal_queue_send(q, &val_tx, 100));
    EXPECT_TRUE(osal_queue_receive(q, &val_rx, 100));
    EXPECT_EQ(val_tx, val_rx);
}

TEST(FreeRTOSOSALTest, TimerStartStop) {
    osal_timer_t tmr = osal_timer_create("test_timer", 100, true, NULL, NULL);
    EXPECT_NE(tmr, nullptr);
    EXPECT_TRUE(osal_timer_start(tmr));
    EXPECT_TRUE(osal_timer_stop(tmr));
}

TEST(FreeRTOSOSALTest, TimeUtilities) {
    uint32_t t1 = osal_get_tick_ms();
    osal_delay_ms(10);
    uint32_t t2 = osal_get_tick_ms();
    EXPECT_GE(t2, t1);
}
