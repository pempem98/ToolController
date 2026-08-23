#include <gtest/gtest.h>
#include "ethercat_soe.h"

TEST(EtherCATSoETest, InitAndTypeCheck) {
    ethercat_soe_t soe;
    EXPECT_FALSE(ethercat_soe_init(NULL));
    EXPECT_TRUE(ethercat_soe_init(&soe));
    ecat_app_protocol_t *proto = ethercat_soe_get_protocol(&soe);
    EXPECT_EQ(proto->type, ECAT_PROTO_SOE);
    EXPECT_EQ(ethercat_soe_get_protocol(NULL), nullptr);

    EXPECT_TRUE(proto->init(proto));
}

TEST(EtherCATSoETest, SetAndGetIDN) {
    ethercat_soe_t soe;
    ethercat_soe_init(&soe);

    // Null guards
    int32_t target_pos = 98765;
    EXPECT_FALSE(ethercat_soe_set_idn(NULL, 36, &target_pos, sizeof(target_pos)));
    EXPECT_FALSE(ethercat_soe_set_idn(&soe, 36, NULL, sizeof(target_pos)));
    EXPECT_FALSE(ethercat_soe_set_idn(&soe, 36, &target_pos, 0));
    EXPECT_FALSE(ethercat_soe_set_idn(&soe, 36, &target_pos, 33)); // > 32 bytes

    EXPECT_TRUE(ethercat_soe_set_idn(&soe, 36, &target_pos, sizeof(target_pos)));

    // Update existing IDN
    int32_t updated_pos = 11111;
    EXPECT_TRUE(ethercat_soe_set_idn(&soe, 36, &updated_pos, sizeof(updated_pos)));

    int32_t read_pos = 0;
    uint16_t len = 0;

    EXPECT_FALSE(ethercat_soe_get_idn(NULL, 36, &read_pos, &len));
    EXPECT_FALSE(ethercat_soe_get_idn(&soe, 36, NULL, &len));
    EXPECT_FALSE(ethercat_soe_get_idn(&soe, 36, &read_pos, NULL));
    EXPECT_FALSE(ethercat_soe_get_idn(&soe, 999, &read_pos, &len)); // missing IDN

    EXPECT_TRUE(ethercat_soe_get_idn(&soe, 36, &read_pos, &len));
    EXPECT_EQ(len, sizeof(int32_t));
    EXPECT_EQ(read_pos, 11111);

    // Fill up to max 16 elements
    for (uint16_t i = 1; i <= 15; i++) {
        uint32_t val = i;
        EXPECT_TRUE(ethercat_soe_set_idn(&soe, i + 100, &val, sizeof(val)));
    }
    // 17th element exceeds capacity
    uint32_t extra = 999;
    EXPECT_FALSE(ethercat_soe_set_idn(&soe, 500, &extra, sizeof(extra)));
}

TEST(EtherCATSoETest, MailboxProcessIDNRead) {
    ethercat_soe_t soe;
    ethercat_soe_init(&soe);
    ecat_app_protocol_t *proto = ethercat_soe_get_protocol(&soe);

    // Null guards
    uint8_t rx_buf[4] = {0x01, 100, 0, 0};
    uint8_t tx_buf[64] = {0};
    uint16_t tx_len = 0;

    EXPECT_FALSE(proto->process_mailbox(NULL, rx_buf, sizeof(rx_buf), tx_buf, &tx_len));
    EXPECT_FALSE(proto->process_mailbox(proto, NULL, sizeof(rx_buf), tx_buf, &tx_len));
    EXPECT_FALSE(proto->process_mailbox(proto, rx_buf, 2, tx_buf, &tx_len));
    EXPECT_FALSE(proto->process_mailbox(proto, rx_buf, sizeof(rx_buf), NULL, &tx_len));
    EXPECT_FALSE(proto->process_mailbox(proto, rx_buf, sizeof(rx_buf), tx_buf, NULL));

    // IDN not set (read returns false)
    EXPECT_FALSE(proto->process_mailbox(proto, rx_buf, sizeof(rx_buf), tx_buf, &tx_len));
    EXPECT_EQ(tx_len, 4);

    // IDN set (read returns true)
    int32_t val = 42;
    ethercat_soe_set_idn(&soe, 100, &val, 4);
    EXPECT_TRUE(proto->process_mailbox(proto, rx_buf, sizeof(rx_buf), tx_buf, &tx_len));
    EXPECT_EQ(tx_len, 8);
}

TEST(EtherCATSoETest, ProcessPDO) {
    ethercat_soe_t soe;
    ethercat_soe_init(&soe);
    ecat_app_protocol_t *proto = ethercat_soe_get_protocol(&soe);

    EXPECT_FALSE(proto->process_pdo(NULL, NULL, NULL, 0));

    uint8_t rx_pdo[4] = {0x00, 0x10, 0x00, 0x00};
    uint8_t tx_pdo[4] = {0};

    // Initialize IDN 51 so get_idn in process_pdo succeeds
    uint32_t act_pos = 555;
    ethercat_soe_set_idn(&soe, 51, &act_pos, 4);

    EXPECT_TRUE(proto->process_pdo(proto, rx_pdo, tx_pdo, 4));
}
