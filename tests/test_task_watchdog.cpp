#include <gtest/gtest.h>
#include "watchdog_manager.h"

static bool mock_wdt_feed_called = false;

static bool mock_wdt_init(wdt_interface *self, uint32_t timeout_ms) {
    (void)self; (void)timeout_ms;
    return true;
}

static void mock_wdt_feed(wdt_interface *self) {
    (void)self;
    mock_wdt_feed_called = true;
}

TEST(WatchdogManagerTest, TaskCheckinAndFeed) {
    wdt_interface_t hw_wdt;
    hw_wdt.init = mock_wdt_init;
    hw_wdt.feed = mock_wdt_feed;

    watchdog_manager_t mgr;
    EXPECT_TRUE(watchdog_manager_init(&mgr, &hw_wdt));

    int8_t task1 = watchdog_register_task(&mgr, 100);
    int8_t task2 = watchdog_register_task(&mgr, 200);
    EXPECT_GE(task1, 0);
    EXPECT_GE(task2, 0);

    // Initial checkin
    watchdog_task_checkin(&mgr, task1, 10);
    watchdog_task_checkin(&mgr, task2, 10);

    mock_wdt_feed_called = false;
    EXPECT_TRUE(watchdog_check_all_alive(&mgr, 50));
    EXPECT_TRUE(mock_wdt_feed_called);

    // Task 1 misses deadline (timeout at 10+100 = 110)
    mock_wdt_feed_called = false;
    EXPECT_FALSE(watchdog_check_all_alive(&mgr, 120));
    EXPECT_FALSE(mock_wdt_feed_called);
}
