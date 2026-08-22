#include <gtest/gtest.h>
#include "watchdog_manager.h"
#include "wdt_mock.h"

class TaskWatchdogTest : public ::testing::Test {
protected:
    void SetUp() override {
        wdt_mock_create(&hw_wdt);
        wdt_mock_reset();
    }

    wdt_interface_t hw_wdt;
    watchdog_manager_t mgr;
};

TEST_F(TaskWatchdogTest, Init_Success) {
    EXPECT_TRUE(watchdog_manager_init(&mgr, &hw_wdt));
    EXPECT_EQ(mgr.task_count, 0);
    EXPECT_EQ(g_mock_wdt_init_count, 1u);
}

TEST_F(TaskWatchdogTest, Init_NullGuards) {
    EXPECT_FALSE(watchdog_manager_init(NULL, &hw_wdt));
    EXPECT_FALSE(watchdog_manager_init(&mgr, NULL));
}

TEST_F(TaskWatchdogTest, RegisterTask_MaxSlots) {
    watchdog_manager_init(&mgr, &hw_wdt);

    for (int i = 0; i < MAX_WATCHDOG_TASKS; i++) {
        int8_t id = watchdog_register_task(&mgr, 100);
        EXPECT_EQ(id, i);
    }
    EXPECT_EQ(mgr.task_count, MAX_WATCHDOG_TASKS);

    EXPECT_EQ(watchdog_register_task(&mgr, 100), -1);
}

TEST_F(TaskWatchdogTest, RegisterTask_NullGuard) {
    EXPECT_EQ(watchdog_register_task(NULL, 100), -1);
}

TEST_F(TaskWatchdogTest, TaskCheckinAndFeed) {
    watchdog_manager_init(&mgr, &hw_wdt);

    int8_t task1 = watchdog_register_task(&mgr, 100);
    int8_t task2 = watchdog_register_task(&mgr, 200);
    EXPECT_GE(task1, 0);
    EXPECT_GE(task2, 0);

    watchdog_task_checkin(&mgr, task1, 10);
    watchdog_task_checkin(&mgr, task2, 10);

    wdt_mock_reset();
    EXPECT_TRUE(watchdog_check_all_alive(&mgr, 50));
    EXPECT_EQ(g_mock_wdt_feed_count, 1u);

    wdt_mock_reset();
    EXPECT_FALSE(watchdog_check_all_alive(&mgr, 120));
    EXPECT_EQ(g_mock_wdt_feed_count, 0u);
}

TEST_F(TaskWatchdogTest, Checkin_InvalidOrUnregisteredTask) {
    watchdog_manager_init(&mgr, &hw_wdt);

    watchdog_task_checkin(&mgr, -1, 100);
    watchdog_task_checkin(&mgr, 0, 100);
    watchdog_task_checkin(&mgr, MAX_WATCHDOG_TASKS, 100);
    watchdog_task_checkin(NULL, 0, 100);
}

TEST_F(TaskWatchdogTest, CheckAllAlive_NullGuards) {
    EXPECT_FALSE(watchdog_check_all_alive(NULL, 100));

    watchdog_manager_init(&mgr, &hw_wdt);
    mgr.hw_wdt = NULL;
    EXPECT_FALSE(watchdog_check_all_alive(&mgr, 100));
}

TEST_F(TaskWatchdogTest, CheckAllAlive_NoRegisteredTasks) {
    watchdog_manager_init(&mgr, &hw_wdt);

    wdt_mock_reset();
    EXPECT_TRUE(watchdog_check_all_alive(&mgr, 100));
    EXPECT_EQ(g_mock_wdt_feed_count, 1u);
}
