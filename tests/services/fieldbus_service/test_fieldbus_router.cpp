#include <gtest/gtest.h>
#include "fieldbus_router.h"
#include "can_mock.h"

static bool g_handler_called = false;
static can_frame_t g_received_frame = {};
static void *g_received_user_data = nullptr;

static void test_frame_handler(const can_frame_t *frame, void *user_data) {
    g_handler_called = true;
    if (frame) g_received_frame = *frame;
    g_received_user_data = user_data;
}

class FieldbusRouterTest : public ::testing::Test {
protected:
    void SetUp() override {
        can_mock_create(&can_dev);
        g_handler_called = false;
        g_received_frame = {};
        g_received_user_data = nullptr;
    }

    can_interface_t can_dev;
    fieldbus_router_t router;
};

TEST_F(FieldbusRouterTest, Init_Success) {
    EXPECT_TRUE(fieldbus_router_init(&router, FIELDBUS_TYPE_CANFD, &can_dev));
    EXPECT_EQ(router.type, FIELDBUS_TYPE_CANFD);
    EXPECT_EQ(router.can_dev, &can_dev);
    EXPECT_EQ(router.handler, nullptr);
    EXPECT_EQ(router.user_data, nullptr);
}

TEST_F(FieldbusRouterTest, Init_NullRouter) {
    EXPECT_FALSE(fieldbus_router_init(NULL, FIELDBUS_TYPE_CANFD, &can_dev));
}

TEST_F(FieldbusRouterTest, RegisterHandler_Success) {
    fieldbus_router_init(&router, FIELDBUS_TYPE_CANFD, &can_dev);
    int dummy_data = 42;
    EXPECT_TRUE(fieldbus_router_register_handler(&router, test_frame_handler, &dummy_data));
    EXPECT_EQ(router.handler, test_frame_handler);
    EXPECT_EQ(router.user_data, &dummy_data);
}

TEST_F(FieldbusRouterTest, RegisterHandler_NullRouter) {
    EXPECT_FALSE(fieldbus_router_register_handler(NULL, test_frame_handler, nullptr));
}

TEST_F(FieldbusRouterTest, Process_NullRouter) {
    EXPECT_FALSE(fieldbus_router_process(NULL));
}

TEST_F(FieldbusRouterTest, Process_NullCanDev) {
    fieldbus_router_init(&router, FIELDBUS_TYPE_CANFD, NULL);
    EXPECT_FALSE(fieldbus_router_process(&router));
}

TEST_F(FieldbusRouterTest, Process_NoDataAvailable) {
    fieldbus_router_init(&router, FIELDBUS_TYPE_CANFD, &can_dev);
    EXPECT_FALSE(fieldbus_router_process(&router));
    EXPECT_FALSE(g_handler_called);
}

static bool mock_can_receive_success(can_interface_t *self, can_frame_t *frame) {
    (void)self;
    if (frame) {
        frame->id = 0x123;
        frame->len = 4;
        frame->data[0] = 0xAA;
    }
    return true;
}

TEST_F(FieldbusRouterTest, Process_DataReceived_NoHandler) {
    can_dev.receive = mock_can_receive_success;
    fieldbus_router_init(&router, FIELDBUS_TYPE_CANFD, &can_dev);

    EXPECT_TRUE(fieldbus_router_process(&router));
    EXPECT_FALSE(g_handler_called);
}

TEST_F(FieldbusRouterTest, Process_DataReceived_WithHandler) {
    can_dev.receive = mock_can_receive_success;
    fieldbus_router_init(&router, FIELDBUS_TYPE_CANFD, &can_dev);
    int user_ctx = 99;
    fieldbus_router_register_handler(&router, test_frame_handler, &user_ctx);

    EXPECT_TRUE(fieldbus_router_process(&router));
    EXPECT_TRUE(g_handler_called);
    EXPECT_EQ(g_received_frame.id, 0x123u);
    EXPECT_EQ(g_received_frame.len, 4);
    EXPECT_EQ(g_received_frame.data[0], 0xAA);
    EXPECT_EQ(g_received_user_data, &user_ctx);
}
