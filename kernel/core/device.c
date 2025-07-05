#include "device.h"
#include <string.h>

#define MAX_DEVICES 32

static device_t devices[MAX_DEVICES];
static uint32_t device_count = 0;

void device_init(void) {
    memset(devices, 0, sizeof(devices));
    device_count = 0;
}

bool device_register(device_t* dev) {
    if (device_count >= MAX_DEVICES) {
        return false;
    }
    devices[device_count] = *dev;
    device_count++;
    return true;
}

device_t* device_get(uint32_t id) {
    for (uint32_t i = 0; i < device_count; i++) {
        if (devices[i].id == id) {
            return &devices[i];
        }
    }
    return NULL;
} 