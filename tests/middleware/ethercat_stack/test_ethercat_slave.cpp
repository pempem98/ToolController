#include <gtest/gtest.h>
#include "ethercat_slave.h"
#include "ethercat_coe.h"
#include "canopen_od.h"

static bool mock_hw_init(ethercat_interface_t *self) {
    (void)self;
    return true;
}

static bool mock_hw_process_pdo(ethercat_interface_t *self, uint8_t *tx_buf, uint8_t *rx_buf, uint16_t len) {
    (void)self; (void)tx_buf; (void)rx_buf; (void)len;
    return true;
}

TEST(EtherCATSlaveTest, InitSuccessAndNullGuards) {
    ethercat_slave_t slave;
    ethercat_interface_t hw = { mock_hw_init, mock_hw_process_pdo, NULL, NULL, NULL };
    canopen_od_t od;
    canopen_od_init(&od);
    ethercat_coe_t coe;
    ethercat_coe_init(&coe, &od);

    EXPECT_FALSE(ethercat_slave_init(NULL, &hw, ethercat_coe_get_protocol(&coe)));
    EXPECT_FALSE(ethercat_slave_init(&slave, NULL, ethercat_coe_get_protocol(&coe)));

    EXPECT_TRUE(ethercat_slave_init(&slave, &hw, ethercat_coe_get_protocol(&coe)));
    EXPECT_EQ(slave.hw, &hw);
    EXPECT_EQ(slave.app_protocol, ethercat_coe_get_protocol(&coe));
}

TEST(EtherCATSlaveTest, SetProtocolSuccessAndNullGuards) {
    ethercat_slave_t slave;
    ethercat_interface_t hw = { mock_hw_init, mock_hw_process_pdo, NULL, NULL, NULL };
    canopen_od_t od;
    canopen_od_init(&od);
    ethercat_coe_t coe;
    ethercat_coe_init(&coe, &od);

    ethercat_slave_init(&slave, &hw, NULL);
    EXPECT_FALSE(ethercat_slave_set_protocol(NULL, ethercat_coe_get_protocol(&coe)));

    EXPECT_TRUE(ethercat_slave_set_protocol(&slave, ethercat_coe_get_protocol(&coe)));
    EXPECT_EQ(slave.app_protocol, ethercat_coe_get_protocol(&coe));
}

TEST(EtherCATSlaveTest, ProcessStateHandling) {
    ethercat_slave_t slave;
    ethercat_interface_t hw = { mock_hw_init, mock_hw_process_pdo, NULL, NULL, NULL };
    canopen_od_t od;
    canopen_od_init(&od);
    ethercat_coe_t coe;
    ethercat_coe_init(&coe, &od);

    EXPECT_FALSE(ethercat_slave_process(NULL));

    EXPECT_TRUE(ethercat_slave_init(&slave, &hw, ethercat_coe_get_protocol(&coe)));
    
    // Default state AL is INIT, process should succeed without PDO call
    EXPECT_TRUE(ethercat_slave_process(&slave));

    // Force current_state to OP and check process_pdo execution
    slave.al.current_state = ECAT_AL_STATE_OP;
    EXPECT_TRUE(ethercat_slave_process(&slave));

    // Force current_state to SAFE_OP
    slave.al.current_state = ECAT_AL_STATE_SAFE_OP;
    EXPECT_TRUE(ethercat_slave_process(&slave));
}
