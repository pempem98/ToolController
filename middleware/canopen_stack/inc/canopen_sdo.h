#ifndef CANOPEN_SDO_H
#define CANOPEN_SDO_H

#include "canopen_types.h"
#include "can_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

// SDO Command Specifiers (CCS / SCS)
typedef enum {
    SDO_CCS_DOWNLOAD_SEGMENT    = 0,
    SDO_CCS_DOWNLOAD_INITIATE   = 1,
    SDO_CCS_UPLOAD_INITIATE     = 2,
    SDO_CCS_UPLOAD_SEGMENT      = 3,
    SDO_CCS_ABORT_TRANSFER      = 4
} sdo_ccs_t;

typedef enum {
    SDO_ABORT_TOGGLE_BIT        = 0x05030000,
    SDO_ABORT_TIMEOUT           = 0x05040000,
    SDO_ABORT_UNKNOWN_COMMAND   = 0x05040001,
    SDO_ABORT_INVALID_PARAM     = 0x06020000,
    SDO_ABORT_NOT_EXISTS        = 0x06020000,
    SDO_ABORT_UNSUPPORTED_ACCESS= 0x06010000,
    SDO_ABORT_GENERAL_ERROR     = 0x08000000
} sdo_abort_code_t;

typedef struct {
    uint8_t          node_id;
    can_interface_t *can_dev;
    bool             fd_enabled;
} canopen_sdo_t;

bool canopen_sdo_init(canopen_sdo_t *sdo, uint8_t node_id, can_interface_t *can_dev, bool fd_enabled);
bool canopen_sdo_write_expedited(canopen_sdo_t *sdo, uint16_t index, uint8_t subindex, const void *data, uint8_t len);
bool canopen_sdo_read_expedited(canopen_sdo_t *sdo, uint16_t index, uint8_t subindex);
bool canopen_sdo_parse_response(const can_frame_t *frame, uint16_t *index, uint8_t *subindex, void *data, uint8_t *len, uint32_t *abort_code);

#ifdef __cplusplus
}
#endif

#endif // CANOPEN_SDO_H
