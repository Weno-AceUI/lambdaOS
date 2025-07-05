#include "config.h"
#include "memory.h"
#include <string.h>

#define MAX_CONFIGS 1024
#define MAX_PROPERTIES 32
#define MAX_VALUES 16

// Configuration system state
static config_t configs[MAX_CONFIGS];
static uint64_t next_config_id = 1;

// Initialize configuration system
void config_init(void) {
    memset(configs, 0, sizeof(configs));
    next_config_id = 1;
}

// Create configuration
uint64_t config_create(const char* name, const char* description) {
    // Find free config slot
    uint64_t config_id = 0;
    for (uint64_t i = 0; i < MAX_CONFIGS; i++) {
        if (!configs[i].active) {
            config_id = i;
            break;
        }
    }

    if (config_id == 0) return 0; // No free slots

    // Initialize config
    config_t* config = &configs[config_id];
    strncpy(config->name, name, sizeof(config->name) - 1);
    strncpy(config->description, description, sizeof(config->description) - 1);
    config->property_count = 0;
    config->active = true;
    config->config_id = next_config_id;

    return next_config_id++;
}

// Add property
bool config_add_property(uint64_t config_id, const char* name, config_type_t type, const void* value, uint64_t size) {
    config_t* config = config_get(config_id);
    if (!config || !config->active) return false;

    // Check if config has space for more properties
    if (config->property_count >= MAX_PROPERTIES) return false;

    // Initialize property
    config_property_t* property = &config->properties[config->property_count++];
    strncpy(property->name, name, sizeof(property->name) - 1);
    property->type = type;
    property->size = size;
    property->active = true;

    // Set property value
    switch (type) {
        case CONFIG_TYPE_INTEGER:
            property->value.integer = *(int64_t*)value;
            break;
        case CONFIG_TYPE_FLOATING:
            property->value.floating = *(double*)value;
            break;
        case CONFIG_TYPE_BOOLEAN:
            property->value.boolean = *(bool*)value;
            break;
        case CONFIG_TYPE_STRING:
            strncpy(property->value.string, (const char*)value, sizeof(property->value.string) - 1);
            break;
        case CONFIG_TYPE_DATA:
            property->value.data = memory_allocate(size);
            if (!property->value.data) {
                config->property_count--;
                return false;
            }
            memcpy(property->value.data, value, size);
            break;
        default:
            config->property_count--;
            return false;
    }

    return true;
}

// Get configuration
config_t* config_get(uint64_t config_id) {
    for (uint64_t i = 0; i < MAX_CONFIGS; i++) {
        if (configs[i].active && configs[i].config_id == config_id) {
            return &configs[i];
        }
    }
    return NULL;
}

// Get property
config_property_t* config_get_property(uint64_t config_id, const char* name) {
    config_t* config = config_get(config_id);
    if (!config || !config->active) return NULL;

    for (uint64_t i = 0; i < config->property_count; i++) {
        if (config->properties[i].active && strcmp(config->properties[i].name, name) == 0) {
            return &config->properties[i];
        }
    }

    return NULL;
}

// Get property value
bool config_get_value(uint64_t config_id, const char* name, void* value, uint64_t* size) {
    if (!value || !size) return false;

    config_property_t* property = config_get_property(config_id, name);
    if (!property) return false;

    switch (property->type) {
        case CONFIG_TYPE_INTEGER:
            if (*size < sizeof(int64_t)) return false;
            *(int64_t*)value = property->value.integer;
            *size = sizeof(int64_t);
            break;
        case CONFIG_TYPE_FLOATING:
            if (*size < sizeof(double)) return false;
            *(double*)value = property->value.floating;
            *size = sizeof(double);
            break;
        case CONFIG_TYPE_BOOLEAN:
            if (*size < sizeof(bool)) return false;
            *(bool*)value = property->value.boolean;
            *size = sizeof(bool);
            break;
        case CONFIG_TYPE_STRING:
            if (*size < strlen(property->value.string) + 1) return false;
            strcpy((char*)value, property->value.string);
            *size = strlen(property->value.string) + 1;
            break;
        case CONFIG_TYPE_DATA:
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
bool config_set_value(uint64_t config_id, const char* name, const void* value, uint64_t size) {
    config_property_t* property = config_get_property(config_id, name);
    if (!property) return false;

    switch (property->type) {
        case CONFIG_TYPE_INTEGER:
            if (size != sizeof(int64_t)) return false;
            property->value.integer = *(int64_t*)value;
            break;
        case CONFIG_TYPE_FLOATING:
            if (size != sizeof(double)) return false;
            property->value.floating = *(double*)value;
            break;
        case CONFIG_TYPE_BOOLEAN:
            if (size != sizeof(bool)) return false;
            property->value.boolean = *(bool*)value;
            break;
        case CONFIG_TYPE_STRING:
            if (size > sizeof(property->value.string)) return false;
            strncpy(property->value.string, (const char*)value, size);
            property->value.string[size] = '\0';
            break;
        case CONFIG_TYPE_DATA:
            if (property->value.data) {
                memory_free(property->value.data, property->size);
            }
            property->value.data = memory_allocate(size);
            if (!property->value.data) return false;
            memcpy(property->value.data, value, size);
            property->size = size;
            break;
        default:
            return false;
    }

    return true;
}

// Get configuration count
uint64_t config_get_count(void) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < MAX_CONFIGS; i++) {
        if (configs[i].active) {
            count++;
        }
    }
    return count;
}

// Get configuration list
bool config_get_list(uint64_t* config_ids, uint64_t* count) {
    if (!config_ids || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_CONFIGS && found < *count; i++) {
        if (configs[i].active) {
            config_ids[found++] = configs[i].config_id;
        }
    }

    *count = found;
    return true;
}

// Get property count
uint64_t config_get_property_count(uint64_t config_id) {
    config_t* config = config_get(config_id);
    return config ? config->property_count : 0;
}

// Get property list
bool config_get_property_list(uint64_t config_id, char** names, uint64_t* count) {
    if (!names || !count) return false;

    config_t* config = config_get(config_id);
    if (!config || !config->active) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < config->property_count && found < *count; i++) {
        if (config->properties[i].active) {
            names[found++] = config->properties[i].name;
        }
    }

    *count = found;
    return true;
}

// Get configuration info
bool config_get_info(uint64_t config_id, config_info_t* info) {
    if (!info) return false;

    config_t* config = config_get(config_id);
    if (!config || !config->active) return false;

    strncpy(info->name, config->name, sizeof(info->name) - 1);
    strncpy(info->description, config->description, sizeof(info->description) - 1);
    info->property_count = config->property_count;

    return true;
}

// Get property info
bool config_get_property_info(uint64_t config_id, const char* name, config_property_info_t* info) {
    if (!info) return false;

    config_property_t* property = config_get_property(config_id, name);
    if (!property) return false;

    strncpy(info->name, property->name, sizeof(info->name) - 1);
    info->type = property->type;
    info->size = property->size;

    return true;
}

// Get system stats
void config_get_system_stats(config_system_stats_t* stats) {
    if (!stats) return;

    stats->total_configs = MAX_CONFIGS;
    stats->active_configs = config_get_count();
    stats->total_properties = 0;
    stats->active_properties = 0;

    for (uint64_t i = 0; i < MAX_CONFIGS; i++) {
        if (configs[i].active) {
            stats->total_properties += MAX_PROPERTIES;
            stats->active_properties += configs[i].property_count;
        }
    }
} 