#ifndef ETHERCAT_TYPES_H
#define ETHERCAT_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// EtherCAT Application Layer States (ETG.1000 standard)
typedef enum {
    ECAT_AL_STATE_NONE       = 0x00,
    ECAT_AL_STATE_INIT       = 0x01,
    ECAT_AL_STATE_PRE_OP     = 0x02,
    ECAT_AL_STATE_BOOTSTRAP  = 0x03,
    ECAT_AL_STATE_SAFE_OP    = 0x04,
    ECAT_AL_STATE_OP         = 0x08
} ecat_al_state_t;

// EtherCAT Application Protocol Types (Mailbox protocols)
typedef enum {
    ECAT_PROTO_AOE  = 0x01, // ADS over EtherCAT
    ECAT_PROTO_EOE  = 0x02, // Ethernet over EtherCAT
    ECAT_PROTO_COE  = 0x04, // CANopen over EtherCAT
    ECAT_PROTO_FOE  = 0x08, // File Access over EtherCAT
    ECAT_PROTO_SOE  = 0x10, // Servo Drive Profile over EtherCAT (SERCOS)
    ECAT_PROTO_VOE  = 0x20  // Vendor specific over EtherCAT
} ecat_protocol_type_t;

#ifdef __cplusplus
}
#endif

#endif // ETHERCAT_TYPES_H
