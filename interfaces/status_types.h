#ifndef STATUS_TYPES_H
#define STATUS_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    STATUS_OK                = 0,
    STATUS_ERROR             = -1,
    STATUS_NOT_IMPLEMENTED   = -2,
    STATUS_BUSY              = -3,
    STATUS_TIMEOUT           = -4,
    STATUS_INVALID_PARAM     = -5
} status_t;

#ifdef __cplusplus
}
#endif

#endif // STATUS_TYPES_H

