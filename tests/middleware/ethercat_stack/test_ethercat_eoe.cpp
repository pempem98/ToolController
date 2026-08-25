#include <gtest/gtest.h>
#include "ethercat_eoe.h"

TEST(EtherCATEoETest, InitAndMACCheck) {
    ethercat_eoe_t eoe;
    EXPECT_FALSE(ethercat_eoe_init(NULL, NULL));

    // Init with NULL MAC (uses default MAC)
    EXPECT_TRUE(ethercat_eoe_init(&eoe, NULL));
    EXPECT_EQ(eoe.mac_address[0], 0x02);
    EXPECT_EQ(eoe.mac_address[5], 0x01);

    // Init with explicit MAC
    uint8_t mac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    EXPECT_TRUE(ethercat_eoe_init(&eoe, mac));
    EXPECT_EQ(eoe.mac_address[0], 0xAA);
    EXPECT_EQ(eoe.mac_address[5], 0xFF);

    ecat_app_protocol_t *proto = ethercat_eoe_get_protocol(&eoe);
    EXPECT_EQ(proto->type, ECAT_PROTO_EOE);
    EXPECT_EQ(ethercat_eoe_get_protocol(NULL), nullptr);

    EXPECT_TRUE(proto->init(proto));
}

TEST(EtherCATEoETest, SendAndReceiveFrames) {
    ethercat_eoe_t eoe;
    ethercat_eoe_init(&eoe, NULL);

    uint8_t tx_frame[16] = {1, 2, 3, 4, 5, 6, 7, 8};
    
    // Null guards
    EXPECT_FALSE(ethercat_eoe_send_frame(NULL, tx_frame, 8));
    EXPECT_FALSE(ethercat_eoe_send_frame(&eoe, NULL, 8));
    EXPECT_FALSE(ethercat_eoe_send_frame(&eoe, tx_frame, 0));
    EXPECT_FALSE(ethercat_eoe_send_frame(&eoe, tx_frame, 2000)); // Exceeds tx_frame_buf capacity

    EXPECT_TRUE(ethercat_eoe_send_frame(&eoe, tx_frame, 8));

    // Receive before mailbox process
    uint8_t rx_out[16] = {0};
    uint16_t rx_len = 0;
    EXPECT_FALSE(ethercat_eoe_receive_frame(NULL, rx_out, &rx_len));
    EXPECT_FALSE(ethercat_eoe_receive_frame(&eoe, NULL, &rx_len));
    EXPECT_FALSE(ethercat_eoe_receive_frame(&eoe, rx_out, NULL));
    EXPECT_FALSE(ethercat_eoe_receive_frame(&eoe, rx_out, &rx_len)); // no rx frame stored yet
}

TEST(EtherCATEoETest, MailboxProcess) {
    ethercat_eoe_t eoe;
    ethercat_eoe_init(&eoe, NULL);
    ecat_app_protocol_t *proto = ethercat_eoe_get_protocol(&eoe);

    uint8_t rx_buf[12] = {0, 0, 0, 0, 10, 20, 30, 40, 50, 60, 70, 80};
    uint8_t tx_buf[100] = {0};
    uint16_t tx_len = 0;

    // Null guards
    EXPECT_FALSE(proto->process_mailbox(NULL, rx_buf, sizeof(rx_buf), tx_buf, &tx_len));
    EXPECT_FALSE(proto->process_mailbox(proto, NULL, sizeof(rx_buf), tx_buf, &tx_len));
    EXPECT_FALSE(proto->process_mailbox(proto, rx_buf, 2, tx_buf, &tx_len));
    EXPECT_FALSE(proto->process_mailbox(proto, rx_buf, sizeof(rx_buf), NULL, &tx_len));
    EXPECT_FALSE(proto->process_mailbox(proto, rx_buf, sizeof(rx_buf), tx_buf, NULL));

    // Process mailbox with no tx frame available
    EXPECT_TRUE(proto->process_mailbox(proto, rx_buf, sizeof(rx_buf), tx_buf, &tx_len));
    EXPECT_EQ(tx_len, 0);

    // Verify stored rx frame can be retrieved
    uint8_t rx_frame[16] = {0};
    uint16_t len = 0;
    EXPECT_TRUE(ethercat_eoe_receive_frame(&eoe, rx_frame, &len));
    EXPECT_EQ(len, 8);
    EXPECT_EQ(rx_frame[0], 10);

    // Queue tx frame and process mailbox
    uint8_t frame_to_send[10] = {0xAA, 0xBB};
    ethercat_eoe_send_frame(&eoe, frame_to_send, 10);
    EXPECT_TRUE(proto->process_mailbox(proto, rx_buf, sizeof(rx_buf), tx_buf, &tx_len));
    EXPECT_EQ(tx_len, 14); // 4 bytes header + 10 bytes frame

    // Process PDO (does nothing for EoE)
    EXPECT_TRUE(proto->process_pdo(proto, NULL, NULL, 0));
}
