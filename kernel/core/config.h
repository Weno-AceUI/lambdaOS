#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

// Configuration types
typedef enum {
    CONFIG_TYPE_INTEGER,
    CONFIG_TYPE_FLOATING,
    CONFIG_TYPE_BOOLEAN,
    CONFIG_TYPE_STRING,
    CONFIG_TYPE_DATA
} config_type_t;

// Configuration property structure
typedef struct {
    char name[32];
    config_type_t type;
    union {
        int64_t integer;
        double floating;
        bool boolean;
        char string[256];
        void* data;
    } value;
    uint64_t size;
    bool active;
} config_property_t;

// Configuration structure
typedef struct {
    char name[32];
    char description[256];
    config_property_t properties[32];  // MAX_PROPERTIES
    uint64_t property_count;
    bool active;
    uint64_t config_id;
} config_t;

// Configuration info structure
typedef struct {
    char name[32];
    char description[256];
    uint64_t property_count;
} config_info_t;

// Property info structure
typedef struct {
    char name[32];
    config_type_t type;
    uint64_t size;
} config_property_info_t;

// System stats structure
typedef struct {
    uint64_t total_configs;
    uint64_t active_configs;
    uint64_t total_properties;
    uint64_t active_properties;
} config_system_stats_t;

// Function declarations
void config_init(void);
uint64_t config_create(const char* name, const char* description);
bool config_add_property(uint64_t config_id, const char* name, config_type_t type, const void* value, uint64_t size);
config_t* config_get(uint64_t config_id);
config_property_t* config_get_property(uint64_t config_id, const char* name);
bool config_get_value(uint64_t config_id, const char* name, void* value, uint64_t* size);
bool config_set_value(uint64_t config_id, const char* name, const void* value, uint64_t size);
uint64_t config_get_count(void);
bool config_get_list(uint64_t* config_ids, uint64_t* count);
uint64_t config_get_property_count(uint64_t config_id);
bool config_get_property_list(uint64_t config_id, char** names, uint64_t* count);
bool config_get_info(uint64_t config_id, config_info_t* info);
bool config_get_property_info(uint64_t config_id, const char* name, config_property_info_t* info);
void config_get_system_stats(config_system_stats_t* stats);

#endif // CONFIG_H 