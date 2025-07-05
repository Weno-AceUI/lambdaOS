#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    DEVICE_TYPE_UNKNOWN,
    DEVICE_TYPE_BLOCK,
    DEVICE_TYPE_CHAR,
    DEVICE_TYPE_NET
} device_type_t;

typedef struct device {
    uint32_t id;
    device_type_t type;
    const char* name;
    void* driver_data;
} device_t;

void device_init(void);
bool device_register(device_t* dev);
device_t* device_get(uint32_t id);

#endif // DEVICE_H 