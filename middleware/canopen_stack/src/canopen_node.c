#include "canopen_node.h"
#include <string.h>

bool canopen_node_init(canopen_node_t *node, uint8_t node_id, can_interface_t *can_hw, const canopen_can_config_t *config) {
    if (!node || !can_hw) return false;

    memset(node, 0, sizeof(canopen_node_t));
    node->node_id = node_id;
    node->can_hw = can_hw;

    if (config) {
        node->config = *config;
    } else {
        node->config.nominal_baudrate = 1000000;
        node->config.data_baudrate = 2000000; // 2 Mbps default for FD as requested
        node->config.fd_enabled = true;
    }

    if (!canopen_od_init(&node->od)) return false;
    if (!canopen_nmt_init(&node->nmt, node_id, can_hw, node->config.fd_enabled)) return false;
    if (!canopen_sdo_init(&node->sdo, node_id, can_hw, node->config.fd_enabled)) return false;
    if (!canopen_pdo_init(&node->pdo, node_id, can_hw, node->config.fd_enabled)) return false;

    return true;
}

bool canopen_node_process(canopen_node_t *node) {
    if (!node || !node->can_hw || !node->can_hw->receive) return false;

    can_frame_t rx_frame;
    if (node->can_hw->receive(node->can_hw, &rx_frame)) {
        if (canopen_nmt_process_msg(&node->nmt, &rx_frame)) {
            return true;
        }
        if (canopen_pdo_process_rx(&node->pdo, &rx_frame)) {
            return true;
        }
        uint16_t idx;
        uint8_t subidx, len;
        if (canopen_sdo_parse_response(&rx_frame, &idx, &subidx, NULL, &len, NULL)) {
            return true;
        }
    }
    return false;
}
