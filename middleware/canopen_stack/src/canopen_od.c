#include "canopen_od.h"
#include <string.h>

bool canopen_od_init(canopen_od_t *od) {
    if (!od) return false;

    memset(od, 0, sizeof(canopen_od_t));

    // Register standard CiA 402 drive objects
    canopen_od_register_entry(od, OD_IDX_CONTROLWORD, 0, sizeof(uint16_t), &od->controlword, false);
    canopen_od_register_entry(od, OD_IDX_STATUSWORD, 0, sizeof(uint16_t), &od->statusword, false);
    canopen_od_register_entry(od, OD_IDX_MODE_OF_OPERATION, 0, sizeof(int8_t), &od->mode_of_operation, false);
    canopen_od_register_entry(od, OD_IDX_MODE_DISPLAY, 0, sizeof(int8_t), &od->mode_display, true);
    canopen_od_register_entry(od, OD_IDX_TARGET_POSITION, 0, sizeof(int32_t), &od->target_position, false);
    canopen_od_register_entry(od, OD_IDX_POSITION_ACTUAL_VALUE, 0, sizeof(int32_t), &od->position_actual, true);
    canopen_od_register_entry(od, OD_IDX_TARGET_VELOCITY, 0, sizeof(int32_t), &od->target_velocity, false);
    canopen_od_register_entry(od, OD_IDX_VELOCITY_ACTUAL_VALUE, 0, sizeof(int32_t), &od->velocity_actual, true);

    return true;
}

bool canopen_od_register_entry(canopen_od_t *od, uint16_t index, uint8_t subindex, uint8_t size, void *data_ptr, bool read_only) {
    if (!od || !data_ptr || size == 0 || od->entry_count >= OD_MAX_ENTRIES) return false;

    od_entry_t *entry = &od->entries[od->entry_count++];
    entry->index = index;
    entry->subindex = subindex;
    entry->size = size;
    entry->data_ptr = data_ptr;
    entry->read_only = read_only;

    return true;
}

bool canopen_od_read(const canopen_od_t *od, uint16_t index, uint8_t subindex, void *buf, uint8_t *len) {
    if (!od || !buf || !len) return false;

    for (uint8_t i = 0; i < od->entry_count; i++) {
        const od_entry_t *entry = &od->entries[i];
        if (entry->index == index && entry->subindex == subindex) {
            memcpy(buf, entry->data_ptr, entry->size);
            *len = entry->size;
            return true;
        }
    }
    return false;
}

bool canopen_od_write(canopen_od_t *od, uint16_t index, uint8_t subindex, const void *buf, uint8_t len) {
    if (!od || !buf || len == 0) return false;

    for (uint8_t i = 0; i < od->entry_count; i++) {
        od_entry_t *entry = &od->entries[i];
        if (entry->index == index && entry->subindex == subindex) {
            if (entry->read_only) return false;
            uint8_t write_len = (len < entry->size) ? len : entry->size;
            memcpy(entry->data_ptr, buf, write_len);
            return true;
        }
    }
    return false;
}
