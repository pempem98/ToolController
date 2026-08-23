#ifndef CANOPEN_NODE_H
#define CANOPEN_NODE_H

#include "canopen_types.h"
#include "canopen_nmt.h"
#include "canopen_sdo.h"
#include "canopen_pdo.h"
#include "canopen_od.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t              node_id;
    canopen_can_config_t config;
    can_interface_t     *can_hw;
    canopen_nmt_t        nmt;
    canopen_sdo_t        sdo;
    canopen_pdo_t        pdo;
    canopen_od_t         od;
} canopen_node_t;

bool canopen_node_init(canopen_node_t *node, uint8_t node_id, can_interface_t *can_hw, const canopen_can_config_t *config);
bool canopen_node_process(canopen_node_t *node);

#ifdef __cplusplus
}
#endif

#endif // CANOPEN_NODE_H
