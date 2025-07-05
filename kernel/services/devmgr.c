#include "devmgr.h"
#include "../drivers/driver.h"
#include "../include/memory.h"
#include <string.h>

#define MAX_DEVICES 1024
#define MAX_DEVICE_PROPERTIES 32

// Device management service state
static device_t devices[MAX_DEVICES];
static uint64_t next_device_id = 1;

// Initialize device management service
void devmgr_init(void) {
    memset(devices, 0, sizeof(devices));
    next_device_id = 1;
}

// Register device type (no-op, since device_type_t is now an enum)
bool devmgr_register_type(const char* name, const char* description) {
    (void)name;
    (void)description;
    return true;
}

// Register device
uint64_t devmgr_register_device(const char* name, device_type_t type, uint64_t driver_id) {
    // Find free device slot
    uint64_t device_id = 0;
    bool found = false;
    for (uint64_t i = 0; i < MAX_DEVICES; i++) {
        if (!devices[i].active) {
            device_id = i;
            found = true;
            break;
        }
    }
    if (!found) return 0; // No free slots

    // Initialize device
    device_t* device = &devices[device_id];
    device->device_id = next_device_id++;
    strncpy(device->name, name, sizeof(device->name) - 1);
    device->type = type;
    device->driver_id = driver_id;
    device->property_count = 0;
    device->active = true;

    return device->device_id;
}

// Add device property
bool devmgr_add_property(uint64_t device_id, const char* name, property_type_t type, const void* value, uint64_t size) {
    device_t* device = devmgr_get_device(device_id);
    if (!device || !device->active) return false;

    // Check if device has space for more properties
    if (device->property_count >= MAX_DEVICE_PROPERTIES) return false;

    // Initialize property
    device_property_t* property = &device->properties[device->property_count++];
    strncpy(property->name, name, sizeof(property->name) - 1);
    property->type = type;
    property->size = size;
    property->active = true;

    // Set property value
    switch (type) {
        case PROPERTY_TYPE_INTEGER:
            property->value.integer = *(int64_t*)value;
            break;
        case PROPERTY_TYPE_FLOATING:
            property->value.floating = *(double*)value;
            break;
        case PROPERTY_TYPE_BOOLEAN:
            property->value.boolean = *(bool*)value;
            break;
        case PROPERTY_TYPE_STRING:
            strncpy(property->value.string, (const char*)value, sizeof(property->value.string) - 1);
            break;
        case PROPERTY_TYPE_DATA:
            property->value.data = memory_alloc(size);
            if (!property->value.data) {
                device->property_count--;
                return false;
            }
            memcpy(property->value.data, value, size);
            break;
        default:
            device->property_count--;
            return false;
    }

    return true;
}

// Get device
device_t* devmgr_get_device(uint64_t device_id) {
    for (uint64_t i = 0; i < MAX_DEVICES; i++) {
        if (devices[i].active && devices[i].device_id == device_id) {
            return &devices[i];
        }
    }
    return NULL;
}

// Get property
device_property_t* devmgr_get_property(uint64_t device_id, const char* name) {
    device_t* device = devmgr_get_device(device_id);
    if (!device || !device->active) return NULL;

    for (uint64_t i = 0; i < device->property_count; i++) {
        if (device->properties[i].active && strcmp(device->properties[i].name, name) == 0) {
            return &device->properties[i];
        }
    }

    return NULL;
}

// Get property value
bool devmgr_get_value(uint64_t device_id, const char* name, void* value, uint64_t* size) {
    if (!value || !size) return false;

    device_property_t* property = devmgr_get_property(device_id, name);
    if (!property) return false;

    switch (property->type) {
        case PROPERTY_TYPE_INTEGER:
            if (*size < sizeof(int64_t)) return false;
            *(int64_t*)value = property->value.integer;
            *size = sizeof(int64_t);
            break;
        case PROPERTY_TYPE_FLOATING:
            if (*size < sizeof(double)) return false;
            *(double*)value = property->value.floating;
            *size = sizeof(double);
            break;
        case PROPERTY_TYPE_BOOLEAN:
            if (*size < sizeof(bool)) return false;
            *(bool*)value = property->value.boolean;
            *size = sizeof(bool);
            break;
        case PROPERTY_TYPE_STRING:
            if (*size < strlen(property->value.string) + 1) return false;
            strcpy((char*)value, property->value.string);
            *size = strlen(property->value.string) + 1;
            break;
        case PROPERTY_TYPE_DATA:
            if (*size < property->size) return false;
            memcpy(value, property->value.data, property->size);
            *size = property->size;
            break;
        default:
            return false;
    }

    return true;
}

// Set property value
bool devmgr_set_value(uint64_t device_id, const char* name, const void* value, uint64_t size) {
    device_property_t* property = devmgr_get_property(device_id, name);
    if (!property) return false;

    switch (property->type) {
        case PROPERTY_TYPE_INTEGER:
            if (size != sizeof(int64_t)) return false;
            property->value.integer = *(int64_t*)value;
            break;
        case PROPERTY_TYPE_FLOATING:
            if (size != sizeof(double)) return false;
            property->value.floating = *(double*)value;
            break;
        case PROPERTY_TYPE_BOOLEAN:
            if (size != sizeof(bool)) return false;
            property->value.boolean = *(bool*)value;
            break;
        case PROPERTY_TYPE_STRING:
            if (size > sizeof(property->value.string)) return false;
            strncpy(property->value.string, (const char*)value, size);
            property->value.string[size] = '\0';
            break;
        case PROPERTY_TYPE_DATA:
            if (property->value.data) {
                memory_free(property->value.data);
            }
            property->value.data = memory_alloc(size);
            if (!property->value.data) return false;
            memcpy(property->value.data, value, size);
            property->size = size;
            break;
        default:
            return false;
    }

    return true;
}

// Get device count
uint64_t devmgr_get_device_count(device_type_t type) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < MAX_DEVICES; i++) {
        if (devices[i].active && devices[i].type == type) {
            count++;
        }
    }
    return count;
}

// Get device list
bool devmgr_get_device_list(device_type_t type, uint64_t* device_ids, uint64_t* count) {
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

// Get property count
uint64_t devmgr_get_property_count(uint64_t device_id) {
    device_t* device = devmgr_get_device(device_id);
    return device ? device->property_count : 0;
}

// Get property list
bool devmgr_get_property_list(uint64_t device_id, char** names, uint64_t* count) {
    if (!names || !count) return false;

    device_t* device = devmgr_get_device(device_id);
    if (!device) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < device->property_count && found < *count; i++) {
        if (device->properties[i].active) {
            names[found++] = device->properties[i].name;
        }
    }

    *count = found;
    return true;
}

// Get device information
bool devmgr_get_device_info(uint64_t device_id, device_t* info) {
    if (!info) return false;

    device_t* device = devmgr_get_device(device_id);
    if (!device) return false;

    strncpy(info->name, device->name, sizeof(info->name) - 1);
    info->type = device->type;
    info->driver_id = device->driver_id;
    info->property_count = device->property_count;

    return true;
}

// Get property information
bool devmgr_get_property_info(uint64_t device_id, const char* name, property_info_t* info) {
    if (!info) return false;

    device_property_t* property = devmgr_get_property(device_id, name);
    if (!property) return false;

    strncpy(info->name, property->name, sizeof(info->name) - 1);
    info->type = property->type;
    info->size = property->size;

    return true;
}

// Get system statistics
void devmgr_get_system_stats(devmgr_system_stats_t* stats) {
    if (!stats) return;

    stats->total_devices = 0;
    stats->active_devices = 0;
    stats->total_properties = 0;
    stats->active_properties = 0;

    for (uint64_t i = 0; i < MAX_DEVICES; i++) {
        stats->total_devices++;
        if (devices[i].active) {
            stats->active_devices++;
            stats->total_properties += devices[i].property_count;
            for (uint64_t j = 0; j < devices[i].property_count; j++) {
                if (devices[i].properties[j].active) {
                    stats->active_properties++;
                }
            }
        }
    }
} 