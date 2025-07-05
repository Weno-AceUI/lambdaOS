#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>
#include <stdbool.h>

// Driver types
typedef enum {
    DRIVER_TYPE_UNKNOWN,
    DRIVER_TYPE_BLOCK,
    DRIVER_TYPE_CHAR,
    DRIVER_TYPE_NETWORK,
    DRIVER_TYPE_DISPLAY,
    DRIVER_TYPE_INPUT
} driver_type_t;

// Device types
typedef enum {
    DEVICE_TYPE_UNKNOWN,
    DEVICE_TYPE_DISK,
    DEVICE_TYPE_KEYBOARD,
    DEVICE_TYPE_MOUSE,
    DEVICE_TYPE_DISPLAY,
    DEVICE_TYPE_NETWORK
} device_type_t;

// Property types
typedef enum {
    PROPERTY_TYPE_INTEGER,
    PROPERTY_TYPE_FLOATING,
    PROPERTY_TYPE_BOOLEAN,
    PROPERTY_TYPE_STRING,
    PROPERTY_TYPE_DATA
} property_type_t;

// Driver operations structure
typedef struct {
    bool (*device_init)(uint64_t device_id, void* private_data);
    void (*device_cleanup)(uint64_t device_id, void* private_data);
    // Add more driver operation function pointers as needed
} driver_ops_t;

// Device property structure
typedef struct {
    char name[32];
    property_type_t type;
    union {
        int64_t integer;
        double floating;
        bool boolean;
        char string[256];
        void* data;
    } value;
    uint64_t size;
    bool active;
} device_property_t;

// Device structure
typedef struct device_t {
    uint64_t device_id;
    char name[32];
    device_type_t type;
    uint64_t driver_id;
    device_property_t properties[32];
    uint64_t property_count;
    void* private_data;
    bool active;
} device_t;

// Device info structure (for API compatibility)
typedef device_t device_info_t;

// Driver structure
typedef struct driver_t {
    uint64_t driver_id;
    char name[32];
    driver_type_t type;
    driver_ops_t ops;
    bool active;
} driver_t;

// Driver info structure (for API compatibility)
typedef driver_t driver_info_t;

// System stats structure
typedef struct {
    uint64_t total_drivers;
    uint64_t active_drivers;
    uint64_t total_devices;
    uint64_t active_devices;
} driver_system_stats_t;

// Function declarations
void driver_init(void);
uint64_t driver_register(const char* name, driver_type_t type, const driver_ops_t* ops);
bool driver_unregister(uint64_t driver_id);
uint64_t device_register(uint64_t driver_id, const char* name, device_type_t type, void* private_data);
bool device_unregister(uint64_t device_id);
driver_t* driver_get(uint64_t driver_id);
device_t* device_get(uint64_t device_id);
uint64_t device_get_by_type(device_type_t type);
uint64_t device_get_count(device_type_t type);
bool device_get_list(device_type_t type, uint64_t* device_ids, uint64_t* count);
uint64_t driver_get_count(driver_type_t type);
bool driver_get_list(driver_type_t type, uint64_t* driver_ids, uint64_t* count);
bool device_get_info(uint64_t device_id, device_info_t* info);
bool driver_get_info(uint64_t driver_id, driver_info_t* info);
void driver_get_system_stats(driver_system_stats_t* stats);

#endif // DRIVER_H 