#ifndef ETHERCAT_INTERFACE_H
#define ETHERCAT_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ethercat_interface {
    bool (*init)(struct ethercat_interface *self);
    bool (*process_pdo)(struct ethercat_interface *self, uint8_t *tx_buf, uint8_t *rx_buf, uint16_t len);
    bool (*read_sdo)(struct ethercat_interface *self, uint16_t index, uint8_t subindex, void *data, uint32_t len);
    bool (*write_sdo)(struct ethercat_interface *self, uint16_t index, uint8_t subindex, const void *data, uint32_t len);
    void *priv_data;
} ethercat_interface_t;

#ifdef __cplusplus
}
#endif

#endif // ETHERCAT_INTERFACE_H
