#ifndef CANOPEN_PDO_H
#define CANOPEN_PDO_H

#include "canopen_types.h"
#include "can_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CANOPEN_PDO_TYPE_RPDO1 = 0,
    CANOPEN_PDO_TYPE_TPDO1,
    CANOPEN_PDO_TYPE_RPDO2,
    CANOPEN_PDO_TYPE_TPDO2,
    CANOPEN_PDO_MAX_COUNT
} canopen_pdo_type_t;

typedef struct {
    uint32_t cob_id;
    uint8_t  len;
    uint8_t  data[64];
    bool     valid;
} canopen_pdo_map_t;

typedef struct {
    uint8_t           node_id;
    can_interface_t  *can_dev;
    bool              fd_enabled;
    canopen_pdo_map_t maps[CANOPEN_PDO_MAX_COUNT];
} canopen_pdo_t;

bool canopen_pdo_init(canopen_pdo_t *pdo, uint8_t node_id, can_interface_t *can_dev, bool fd_enabled);
bool canopen_pdo_send_tpdo(canopen_pdo_t *pdo, canopen_pdo_type_t tpdo_num, const uint8_t *data, uint8_t len);
bool canopen_pdo_process_rx(canopen_pdo_t *pdo, const can_frame_t *frame);
bool canopen_pdo_get_rpdo_data(const canopen_pdo_t *pdo, canopen_pdo_type_t rpdo_num, uint8_t *data, uint8_t *len);

#ifdef __cplusplus
}
#endif

#endif // CANOPEN_PDO_H
