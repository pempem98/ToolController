#include <gtest/gtest.h>
#include "heartbeat_service.h"

TEST(HeartbeatServiceTest, Init_SetsDefaults) {
    heartbeat_service_t hb;
    heartbeat_init(&hb, 500);

    EXPECT_EQ(hb.last_heartbeat_timestamp, 0u);
    EXPECT_EQ(hb.timeout_threshold_ms, 500u);
    EXPECT_EQ(hb.heartbeat_counter, 0u);
}

TEST(HeartbeatServiceTest, Init_NullSafe) {
    heartbeat_init(NULL, 500);
}

TEST(HeartbeatServiceTest, RxSignal_UpdatesTimestamp) {
    heartbeat_service_t hb;
    heartbeat_init(&hb, 500);

    heartbeat_rx_signal(&hb, 100);
    EXPECT_EQ(hb.last_heartbeat_timestamp, 100u);
}

TEST(HeartbeatServiceTest, RxSignal_IncrementsCounter) {
    heartbeat_service_t hb;
    heartbeat_init(&hb, 500);

    heartbeat_rx_signal(&hb, 100);
    heartbeat_rx_signal(&hb, 200);
    heartbeat_rx_signal(&hb, 300);

    EXPECT_EQ(hb.heartbeat_counter, 3u);
    EXPECT_EQ(hb.last_heartbeat_timestamp, 300u);
}

TEST(HeartbeatServiceTest, RxSignal_NullSafe) {
    heartbeat_rx_signal(NULL, 100);
}

TEST(HeartbeatServiceTest, IsAlive_WithinTimeout) {
    heartbeat_service_t hb;
    heartbeat_init(&hb, 500);

    heartbeat_rx_signal(&hb, 100);
    EXPECT_TRUE(heartbeat_is_alive(&hb, 599));
}

TEST(HeartbeatServiceTest, IsAlive_ExactBoundary) {
    heartbeat_service_t hb;
    heartbeat_init(&hb, 500);

    heartbeat_rx_signal(&hb, 100);
    EXPECT_TRUE(heartbeat_is_alive(&hb, 600));
}

TEST(HeartbeatServiceTest, IsAlive_Expired) {
    heartbeat_service_t hb;
    heartbeat_init(&hb, 500);

    heartbeat_rx_signal(&hb, 100);
    EXPECT_FALSE(heartbeat_is_alive(&hb, 601));
}

TEST(HeartbeatServiceTest, IsAlive_NullSafe) {
    EXPECT_FALSE(heartbeat_is_alive(NULL, 100));
}

TEST(HeartbeatServiceTest, IsAlive_NoSignalReceived) {
    heartbeat_service_t hb;
    heartbeat_init(&hb, 500);

    EXPECT_TRUE(heartbeat_is_alive(&hb, 500));
    EXPECT_FALSE(heartbeat_is_alive(&hb, 501));
}
