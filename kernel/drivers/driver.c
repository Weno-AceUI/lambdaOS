#include "driver.h"
#include "memory.h"
#include <string.h>

#define MAX_DRIVERS 256
#define MAX_DEVICES 1024

// Driver framework state
static driver_t drivers[MAX_DRIVERS];
static device_t devices[MAX_DEVICES];
static uint64_t next_driver_id = 1;
static uint64_t next_device_id = 1;

// Initialize driver framework
void driver_init(void) {
    memset(drivers, 0, sizeof(drivers));
    memset(devices, 0, sizeof(devices));
    next_driver_id = 1;
    next_device_id = 1;
}

// Register driver
uint64_t driver_register(const char* name, driver_type_t type, const driver_ops_t* ops) {
    // Find free driver slot
    uint64_t driver_id = 0;
    for (uint64_t i = 0; i < MAX_DRIVERS; i++) {
        if (!drivers[i].active) {
            driver_id = i;
            break;
        }
    }

    if (driver_id == 0) return 0; // No free slots

    // Initialize driver
    driver_t* driver = &drivers[driver_id];
    driver->driver_id = next_driver_id++;
    strncpy(driver->name, name, sizeof(driver->name) - 1);
    driver->type = type;
    memcpy(&driver->ops, ops, sizeof(driver_ops_t));
    driver->active = true;

    return driver->driver_id;
}

// Unregister driver
bool driver_unregister(uint64_t driver_id) {
    driver_t* driver = driver_get(driver_id);
    if (!driver || !driver->active) return false;

    // Check if driver has active devices
    for (uint64_t i = 0; i < MAX_DEVICES; i++) {
        if (devices[i].active && devices[i].driver_id == driver_id) {
            return false; // Cannot unregister driver with active devices
        }
    }

    driver->active = false;
    return true;
}

// Register device
uint64_t device_register(uint64_t driver_id, const char* name, device_type_t type, void* private_data) {
    driver_t* driver = driver_get(driver_id);
    if (!driver || !driver->active) return 0;

    // Find free device slot
    uint64_t device_id = 0;
    for (uint64_t i = 0; i < MAX_DEVICES; i++) {
        if (!devices[i].active) {
            device_id = i;
            break;
        }
    }

    if (device_id == 0) return 0; // No free slots

    // Initialize device
    device_t* device = &devices[device_id];
    device->device_id = next_device_id++;
    device->driver_id = driver_id;
    strncpy(device->name, name, sizeof(device->name) - 1);
    device->type = type;
    device->private_data = private_data;
    device->active = true;

    // Call driver's device init function
    if (driver->ops.device_init) {
        if (!driver->ops.device_init(device->device_id, private_data)) {
            device->active = false;
            return 0;
        }
    }

    return device->device_id;
}

// Unregister device
bool device_unregister(uint64_t device_id) {
    device_t* device = device_get(device_id);
    if (!device || !device->active) return false;

    driver_t* driver = driver_get(device->driver_id);
    if (!driver) return false;

    // Call driver's device cleanup function
    if (driver->ops.device_cleanup) {
        driver->ops.device_cleanup(device_id, device->private_data);
    }

    device->active = false;
    return true;
}

// Get driver
driver_t* driver_get(uint64_t driver_id) {
    for (uint64_t i = 0; i < MAX_DRIVERS; i++) {
        if (drivers[i].active && drivers[i].driver_id == driver_id) {
            return &drivers[i];
        }
    }
    return NULL;
}

// Get device
device_t* device_get(uint64_t device_id) {
    for (uint64_t i = 0; i < MAX_DEVICES; i++) {
        if (devices[i].active && devices[i].device_id == device_id) {
            return &devices[i];
        }
    }
    return NULL;
}

// Get device by type
uint64_t device_get_by_type(device_type_t type) {
    for (uint64_t i = 0; i < MAX_DEVICES; i++) {
        if (devices[i].active && devices[i].type == type) {
            return devices[i].device_id;
        }
    }
    return 0;
}

// Get device count
uint64_t device_get_count(device_type_t type) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < MAX_DEVICES; i++) {
        if (devices[i].active && devices[i].type == type) {
            count++;
        }
    }
    return count;
}

// Get device list
bool device_get_list(device_type_t type, uint64_t* device_ids, uint64_t* count) {
    if (!device_ids || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_DEVICES && found < *count; i++) {
        if (devices[i].active && devices[i].type == type) {
            device_ids[found++] = devices[i].device_id;
        }
    }

    *count = found;
    return true;
}

// Get driver count
uint64_t driver_get_count(driver_type_t type) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < MAX_DRIVERS; i++) {
        if (drivers[i].active && drivers[i].type == type) {
            count++;
        }
    }
    return count;
}

// Get driver list
bool driver_get_list(driver_type_t type, uint64_t* driver_ids, uint64_t* count) {
    if (!driver_ids || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_DRIVERS && found < *count; i++) {
        if (drivers[i].active && drivers[i].type == type) {
            driver_ids[found++] = drivers[i].driver_id;
        }
    }

    *count = found;
    return true;
}

// Get device information
bool device_get_info(uint64_t device_id, device_info_t* info) {
    if (!info) return false;

    device_t* device = device_get(device_id);
    if (!device) return false;

    info->device_id = device->device_id;
    info->driver_id = device->driver_id;
    strncpy(info->name, device->name, sizeof(info->name) - 1);
    info->type = device->type;

    return true;
}

// Get driver information
bool driver_get_info(uint64_t driver_id, driver_info_t* info) {
    if (!info) return false;

    driver_t* driver = driver_get(driver_id);
    if (!driver) return false;

    info->driver_id = driver->driver_id;
    strncpy(info->name, driver->name, sizeof(info->name) - 1);
    info->type = driver->type;

    return true;
}

// Get system statistics
void driver_get_system_stats(driver_system_stats_t* stats) {
    if (!stats) return;

    stats->total_drivers = 0;
    stats->active_drivers = 0;
    stats->total_devices = 0;
    stats->active_devices = 0;

    for (uint64_t i = 0; i < MAX_DRIVERS; i++) {
        stats->total_drivers++;
        if (drivers[i].active) {
            stats->active_drivers++;
        }
    }

    for (uint64_t i = 0; i < MAX_DEVICES; i++) {
        stats->total_devices++;
        if (devices[i].active) {
            stats->active_devices++;
        }
    }
} 