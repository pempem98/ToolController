#include <gtest/gtest.h>
#include "ethercat_coe.h"
#include "canopen_od.h"

TEST(EtherCATCoETest, InitAndTypeCheck) {
    canopen_od_t od;
    canopen_od_init(&od);

    ethercat_coe_t coe;
    EXPECT_FALSE(ethercat_coe_init(NULL, &od));
    EXPECT_FALSE(ethercat_coe_init(&coe, NULL));
    EXPECT_TRUE(ethercat_coe_init(&coe, &od));

    ecat_app_protocol_t *proto = ethercat_coe_get_protocol(&coe);
    EXPECT_EQ(proto->type, ECAT_PROTO_COE);
    EXPECT_EQ(ethercat_coe_get_protocol(NULL), nullptr);

    EXPECT_TRUE(proto->init(proto));
}

TEST(EtherCATCoETest, MailboxProcessSDOReadAndNotFound) {
    canopen_od_t od;
    canopen_od_init(&od);

    ethercat_coe_t coe;
    ethercat_coe_init(&coe, &od);
    ecat_app_protocol_t *proto = ethercat_coe_get_protocol(&coe);

    // Null guards for mailbox process
    uint8_t rx_buf[8] = {0x00, 0x7A, 0x60, 0x00};
    uint8_t tx_buf[64] = {0};
    uint16_t tx_len = 0;

    EXPECT_FALSE(proto->process_mailbox(NULL, rx_buf, sizeof(rx_buf), tx_buf, &tx_len));
    EXPECT_FALSE(proto->process_mailbox(proto, NULL, sizeof(rx_buf), tx_buf, &tx_len));
    EXPECT_FALSE(proto->process_mailbox(proto, rx_buf, 4, tx_buf, &tx_len));
    EXPECT_FALSE(proto->process_mailbox(proto, rx_buf, sizeof(rx_buf), NULL, &tx_len));
    EXPECT_FALSE(proto->process_mailbox(proto, rx_buf, sizeof(rx_buf), tx_buf, NULL));

    // Valid OD read (0x607A Target Position)
    EXPECT_TRUE(proto->process_mailbox(proto, rx_buf, sizeof(rx_buf), tx_buf, &tx_len));
    EXPECT_EQ(tx_len, 8);

    // Invalid OD read (0x1234 Object Not Found)
    uint8_t rx_invalid[8] = {0x00, 0x34, 0x12, 0x00};
    EXPECT_FALSE(proto->process_mailbox(proto, rx_invalid, sizeof(rx_invalid), tx_buf, &tx_len));
    EXPECT_EQ(tx_len, 8);
    EXPECT_EQ(tx_buf[6], 0x02);
    EXPECT_EQ(tx_buf[7], 0x06);
}

TEST(EtherCATCoETest, PDOProcessImpl) {
    canopen_od_t od;
    canopen_od_init(&od);

    ethercat_coe_t coe;
    ethercat_coe_init(&coe, &od);
    ecat_app_protocol_t *proto = ethercat_coe_get_protocol(&coe);

    EXPECT_FALSE(proto->process_pdo(NULL, NULL, NULL, 0));

    uint8_t rx_pdo[4] = {0x10, 0x27, 0x00, 0x00}; // 10000 pos
    uint8_t tx_pdo[4] = {0};

    EXPECT_TRUE(proto->process_pdo(proto, rx_pdo, tx_pdo, 4));
}
