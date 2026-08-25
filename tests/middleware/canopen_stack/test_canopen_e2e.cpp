#include <gtest/gtest.h>
#include "canopen_nmt.h"
#include "canopen_sdo.h"
#include "canopen_pdo.h"
#include "can_interface.h"
#include <cstring>

// Simulating an in-memory CAN bus loopback ring-buffer
#define BUS_QUEUE_CAPACITY 16
typedef struct {
    can_frame_t frames[BUS_QUEUE_CAPACITY];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} bus_queue_t;

static bus_queue_t g_can_bus_queue;

static void bus_queue_reset(void) {
    g_can_bus_queue.head = 0;
    g_can_bus_queue.tail = 0;
    g_can_bus_queue.count = 0;
}

static bool bus_send(can_interface_t *self, const can_frame_t *frame) {
    (void)self;
    if (!frame || g_can_bus_queue.count >= BUS_QUEUE_CAPACITY) return false;
    g_can_bus_queue.frames[g_can_bus_queue.tail] = *frame;
    g_can_bus_queue.tail = (g_can_bus_queue.tail + 1) % BUS_QUEUE_CAPACITY;
    g_can_bus_queue.count++;
    return true;
}

static bool bus_receive(can_interface_t *self, can_frame_t *frame) {
    (void)self;
    if (!frame || g_can_bus_queue.count == 0) return false;
    *frame = g_can_bus_queue.frames[g_can_bus_queue.head];
    g_can_bus_queue.head = (g_can_bus_queue.head + 1) % BUS_QUEUE_CAPACITY;
    g_can_bus_queue.count--;
    return true;
}

class CANopenE2ETest : public ::testing::Test {
protected:
    void SetUp() override {
        bus_queue_reset();

        can_dev.init = [](can_interface_t*, uint32_t) { return true; };
        can_dev.send = bus_send;
        can_dev.receive = bus_receive;
        can_dev.set_filter = [](can_interface_t*, uint32_t, uint32_t) { return true; };

        uint8_t node_id = 1;
        canopen_nmt_init(&nmt_master, node_id, &can_dev, false);
        canopen_nmt_init(&nmt_slave, node_id, &can_dev, false);
        canopen_sdo_init(&sdo_master, node_id, &can_dev, false);
        canopen_pdo_init(&pdo_master, node_id, &can_dev, false);
        canopen_pdo_init(&pdo_slave, node_id, &can_dev, false);

        // Configure Slave RPDO1 COB-ID to listen to Master's TPDO1 (0x181)
        pdo_slave.maps[CANOPEN_PDO_TYPE_RPDO1].cob_id = 0x181;
    }

    can_interface_t can_dev;
    canopen_nmt_t nmt_master;
    canopen_nmt_t nmt_slave;
    canopen_sdo_t sdo_master;
    canopen_pdo_t pdo_master;
    canopen_pdo_t pdo_slave;
};

// Integration Test 1: NMT State Transition Flow (Start Remote Node)
TEST_F(CANopenE2ETest, Nmt_StateTransitionFlow) {
    uint8_t target_node = 1;

    EXPECT_EQ(canopen_nmt_get_state(&nmt_slave), CANOPEN_NMT_STATE_PRE_OPERATIONAL);

    EXPECT_TRUE(canopen_nmt_send_command(&nmt_master, target_node, CANOPEN_NMT_CMD_START_NODE));

    can_frame_t frame;
    EXPECT_TRUE(can_dev.receive(&can_dev, &frame));
    EXPECT_EQ(frame.id, 0x000u);
    EXPECT_TRUE(canopen_nmt_process_msg(&nmt_slave, &frame));

    EXPECT_EQ(canopen_nmt_get_state(&nmt_slave), CANOPEN_NMT_STATE_OPERATIONAL);
}

// Integration Test 2: SDO Expedited Write & Response Parsing
TEST_F(CANopenE2ETest, Sdo_WriteExpeditedFlow) {
    uint16_t index = 0x6040;
    uint8_t subindex = 0x00;
    uint16_t value_to_write = 0x000F;

    EXPECT_TRUE(canopen_sdo_write_expedited(&sdo_master, index, subindex, &value_to_write, sizeof(value_to_write)));

    can_frame_t req_frame;
    EXPECT_TRUE(can_dev.receive(&can_dev, &req_frame));
    EXPECT_EQ(req_frame.id, 0x600u + 1); // Master SDO Request COB-ID = 0x601

    // Simulating Slave SDO Write Response (Command Specifier 0x60 = Download Initiate Response)
    can_frame_t resp_frame;
    memset(&resp_frame, 0, sizeof(resp_frame));
    resp_frame.id = 0x580u + 1; // SDO Response COB-ID = 0x581
    resp_frame.len = 8;
    resp_frame.data[0] = (SDO_CCS_DOWNLOAD_INITIATE << 5);
    resp_frame.data[1] = (uint8_t)(index & 0xFF);
    resp_frame.data[2] = (uint8_t)((index >> 8) & 0xFF);
    resp_frame.data[3] = subindex;

    uint16_t rx_index = 0;
    uint8_t rx_subindex = 0;
    uint8_t rx_data[4] = {0};
    uint8_t rx_len = 0;
    uint32_t abort_code = 0;

    EXPECT_TRUE(canopen_sdo_parse_response(&resp_frame, &rx_index, &rx_subindex, rx_data, &rx_len, &abort_code));
    EXPECT_EQ(rx_index, index);
    EXPECT_EQ(rx_subindex, subindex);
    EXPECT_EQ(abort_code, 0u);
}

// Integration Test 3: PDO Transmit & Receive Flow (TPDO Master -> RPDO Slave)
TEST_F(CANopenE2ETest, Pdo_BidirectionalFlow) {
    uint16_t controlword = 0x000F;
    EXPECT_TRUE(canopen_pdo_send_tpdo(&pdo_master, CANOPEN_PDO_TYPE_TPDO1, (uint8_t*)&controlword, sizeof(controlword)));

    can_frame_t pdo_frame;
    EXPECT_TRUE(can_dev.receive(&can_dev, &pdo_frame));
    EXPECT_EQ(pdo_frame.id, 0x181u); // TPDO1 COB-ID = 0x180 + node_id

    EXPECT_TRUE(canopen_pdo_process_rx(&pdo_slave, &pdo_frame));

    uint8_t slave_rx_buf[64] = {0};
    uint8_t slave_rx_len = 0;
    EXPECT_TRUE(canopen_pdo_get_rpdo_data(&pdo_slave, CANOPEN_PDO_TYPE_RPDO1, slave_rx_buf, &slave_rx_len));
    EXPECT_EQ(slave_rx_len, 2);

    uint16_t slave_read_controlword = 0;
    memcpy(&slave_read_controlword, slave_rx_buf, 2);
    EXPECT_EQ(slave_read_controlword, controlword);
}
