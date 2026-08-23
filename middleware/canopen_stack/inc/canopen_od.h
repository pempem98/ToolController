#ifndef CANOPEN_OD_H
#define CANOPEN_OD_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// CiA 402 standard object dictionary indices
#define OD_IDX_CONTROLWORD           0x6040
#define OD_IDX_STATUSWORD            0x6041
#define OD_IDX_MODE_OF_OPERATION     0x6060
#define OD_IDX_MODE_DISPLAY          0x6061
#define OD_IDX_POSITION_ACTUAL_VALUE 0x6064
#define OD_IDX_TARGET_POSITION       0x607A
#define OD_IDX_VELOCITY_ACTUAL_VALUE 0x606C
#define OD_IDX_TARGET_VELOCITY       0x60FF

#define OD_MAX_ENTRIES               16

typedef struct {
    uint16_t index;
    uint8_t  subindex;
    uint8_t  size;
    void    *data_ptr;
    bool     read_only;
} od_entry_t;

typedef struct {
    od_entry_t entries[OD_MAX_ENTRIES];
    uint8_t    entry_count;

    // Default storage buffers for CiA 402 drive variables
    uint16_t controlword;
    uint16_t statusword;
    int8_t   mode_of_operation;
    int8_t   mode_display;
    int32_t  target_position;
    int32_t  position_actual;
    int32_t  target_velocity;
    int32_t  velocity_actual;
} canopen_od_t;

bool canopen_od_init(canopen_od_t *od);
bool canopen_od_register_entry(canopen_od_t *od, uint16_t index, uint8_t subindex, uint8_t size, void *data_ptr, bool read_only);
bool canopen_od_read(const canopen_od_t *od, uint16_t index, uint8_t subindex, void *buf, uint8_t *len);
bool canopen_od_write(canopen_od_t *od, uint16_t index, uint8_t subindex, const void *buf, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif // CANOPEN_OD_H
