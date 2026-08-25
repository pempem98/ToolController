#include <gtest/gtest.h>
#include "canopen_pdo.h"
#include "can_mock.h"

TEST(CANopenPDOTest, SendTPDO1) {
    can_interface_t mock;
    can_mock_create(&mock);

    canopen_pdo_t pdo;
    EXPECT_TRUE(canopen_pdo_init(&pdo, 1, &mock, false));

    uint16_t controlword = 0x000F;
    EXPECT_TRUE(canopen_pdo_send_tpdo(&pdo, CANOPEN_PDO_TYPE_TPDO1, (uint8_t*)&controlword, sizeof(controlword)));
}

TEST(CANopenPDOTest, ReceiveRPDO1Data) {
    can_interface_t mock;
    can_mock_create(&mock);

    canopen_pdo_t pdo;
    canopen_pdo_init(&pdo, 1, &mock, false);

    can_frame_t rx_frame;
    rx_frame.id = CANOPEN_COB_RPDO1_BASE + 1;
    rx_frame.len = 2;
    uint16_t statusword = 0x0027;
    memcpy(rx_frame.data, &statusword, 2);

    EXPECT_TRUE(canopen_pdo_process_rx(&pdo, &rx_frame));

    uint8_t rx_data[64] = {0};
    uint8_t rx_len = 0;
    EXPECT_TRUE(canopen_pdo_get_rpdo_data(&pdo, CANOPEN_PDO_TYPE_RPDO1, rx_data, &rx_len));
    EXPECT_EQ(rx_len, 2);

    uint16_t read_statusword;
    memcpy(&read_statusword, rx_data, 2);
    EXPECT_EQ(read_statusword, 0x0027);
}
