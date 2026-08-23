#include "bsp_adapters.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef __linux__
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#endif

typedef struct {
    uint32_t baudrate;
    uint32_t tx_count;
    uint32_t rx_count;
    can_frame_t rx_queue[16];
    uint8_t rx_head;
    uint8_t rx_tail;
    uint8_t rx_cnt;
    int socket_fd;
    bool is_socket_active;
} sim_can_priv_t;

static sim_can_priv_t g_sim_can_priv;

static bool can_sim_init(can_interface_t *self, uint32_t baudrate) {
    (void)self;
    g_sim_can_priv.baudrate = baudrate;
    g_sim_can_priv.tx_count = 0;
    g_sim_can_priv.rx_count = 0;
    g_sim_can_priv.rx_head = 0;
    g_sim_can_priv.rx_tail = 0;
    g_sim_can_priv.rx_cnt = 0;
    g_sim_can_priv.socket_fd = -1;
    g_sim_can_priv.is_socket_active = false;

#ifdef __linux__
    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s >= 0) {
        struct ifreq ifr;
        strcpy(ifr.ifr_name, "vcan0");
        if (ioctl(s, SIOCGIFINDEX, &ifr) >= 0) {
            struct sockaddr_can addr;
            memset(&addr, 0, sizeof(addr));
            addr.can_family = AF_CAN;
            addr.can_ifindex = ifr.ifr_index;
            if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) >= 0) {
                g_sim_can_priv.socket_fd = s;
                g_sim_can_priv.is_socket_active = true;
                printf("[SIM CAN] Connected to Linux SocketCAN (vcan0)\n");
            } else {
                close(s);
            }
        } else {
            close(s);
        }
    }
#endif

    if (!g_sim_can_priv.is_socket_active) {
        printf("[SIM CAN] Initialized with in-memory loopback (baudrate: %u bps)\n", (unsigned int)baudrate);
    }
    return true;
}

static bool can_sim_send(can_interface_t *self, const can_frame_t *frame) {
    (void)self;
    if (!frame) return false;
    g_sim_can_priv.tx_count++;

#ifdef __linux__
    if (g_sim_can_priv.is_socket_active) {
        struct can_frame linux_frame;
        memset(&linux_frame, 0, sizeof(linux_frame));
        linux_frame.can_id = frame->id;
        if (frame->is_extended) linux_frame.can_id |= CAN_EFF_FLAG;
        linux_frame.can_dlc = frame->len;
        memcpy(linux_frame.data, frame->data, frame->len > 8 ? 8 : frame->len);
        
        ssize_t bytes_sent = write(g_sim_can_priv.socket_fd, &linux_frame, sizeof(linux_frame));
        if (bytes_sent > 0) {
            printf("[SIM CAN vcan0 TX #%u] ID: 0x%08X, Len: %u\n",
                   (unsigned int)g_sim_can_priv.tx_count, (unsigned int)frame->id, frame->len);
            return true;
        }
    }
#endif

    printf("[SIM CAN TX #%u] ID: 0x%08X, Len: %u, FD: %d, Ext: %d\n",
           (unsigned int)g_sim_can_priv.tx_count, (unsigned int)frame->id, frame->len, frame->is_fd, frame->is_extended);
    return true;
}

static bool can_sim_receive(can_interface_t *self, can_frame_t *frame) {
    (void)self;
    if (!frame) return false;

#ifdef __linux__
    if (g_sim_can_priv.is_socket_active) {
        struct can_frame linux_frame;
        ssize_t nbytes = read(g_sim_can_priv.socket_fd, &linux_frame, sizeof(struct can_frame));
        if (nbytes > 0) {
            frame->id = linux_frame.can_id & CAN_EFF_MASK;
            frame->is_extended = (linux_frame.can_id & CAN_EFF_FLAG) != 0;
            frame->len = linux_frame.can_dlc;
            memcpy(frame->data, linux_frame.data, linux_frame.can_dlc);
            frame->is_fd = false;
            g_sim_can_priv.rx_count++;
            return true;
        }
        return false;
    }
#endif

    if (g_sim_can_priv.rx_cnt == 0) return false;
    *frame = g_sim_can_priv.rx_queue[g_sim_can_priv.rx_head];
    g_sim_can_priv.rx_head = (g_sim_can_priv.rx_head + 1) % 16;
    g_sim_can_priv.rx_cnt--;
    g_sim_can_priv.rx_count++;
    return true;
}

static bool can_sim_set_filter(can_interface_t *self, uint32_t id, uint32_t mask) {
    (void)self;
    printf("[SIM CAN] Filter set - ID: 0x%X, Mask: 0x%X\n", (unsigned int)id, (unsigned int)mask);
    return true;
}

void bsp_can_adapter_create(can_interface_t *adapter) {
    if (!adapter) return;
    adapter->init = can_sim_init;
    adapter->send = can_sim_send;
    adapter->receive = can_sim_receive;
    adapter->set_filter = can_sim_set_filter;
    adapter->priv_data = &g_sim_can_priv;
}
