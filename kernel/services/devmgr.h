#ifndef DEVMGR_H
#define DEVMGR_H

#include <stdint.h>
#include <stdbool.h>
#include "../drivers/driver.h"

// Property info structure
typedef struct {
    char name[32];
    property_type_t type;
    uint64_t size;
} property_info_t;

// System stats structure
typedef struct {
    uint64_t total_devices;
    uint64_t active_devices;
    uint64_t total_types;
    uint64_t active_types;
    uint64_t total_properties;
    uint64_t active_properties;
} devmgr_system_stats_t;

// Function declarations
void devmgr_init(void);
bool devmgr_register_type(const char* name, const char* description);
uint64_t devmgr_register_device(const char* name, device_type_t type, uint64_t driver_id);
bool devmgr_add_property(uint64_t device_id, const char* name, property_type_t type, const void* value, uint64_t size);
device_t* devmgr_get_device(uint64_t device_id);
device_property_t* devmgr_get_property(uint64_t device_id, const char* name);
bool devmgr_get_value(uint64_t device_id, const char* name, void* value, uint64_t* size);
bool devmgr_set_value(uint64_t device_id, const char* name, const void* value, uint64_t size);
uint64_t devmgr_get_device_count(device_type_t type);
bool devmgr_get_device_list(device_type_t type, uint64_t* device_ids, uint64_t* count);
uint64_t devmgr_get_property_count(uint64_t device_id);
bool devmgr_get_property_list(uint64_t device_id, char** names, uint64_t* count);
bool devmgr_get_device_info(uint64_t device_id, device_t* info);
bool devmgr_get_property_info(uint64_t device_id, const char* name, property_info_t* info);
void devmgr_get_system_stats(devmgr_system_stats_t* stats);

#endif // DEVMGR_H 