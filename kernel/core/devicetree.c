#include "devicetree.h"
#include "memory.h"
#include <string.h>

#define MAX_NODES 1024
#define MAX_PROPERTIES 32
#define MAX_CHILDREN 16

// Device tree property structure
typedef struct {
    char name[32];
    uint64_t size;
    void* data;
    bool active;
} devicetree_property_t;

// Device tree node structure
typedef struct {
    char name[32];
    char compatible[32];
    uint64_t address;
    uint64_t size;
    devicetree_property_t properties[MAX_PROPERTIES];
    uint64_t property_count;
    uint64_t children[MAX_CHILDREN];
    uint64_t child_count;
    bool active;
} devicetree_node_t;

// Device tree state
static devicetree_node_t nodes[MAX_NODES];
static uint64_t next_node_id = 1;
static uint64_t root_node_id = 0;

// Initialize device tree
void devicetree_init(void) {
    memset(nodes, 0, sizeof(nodes));
    next_node_id = 1;
    root_node_id = 0;
}

// Create root node
uint64_t devicetree_create_root(const char* name, const char* compatible) {
    if (root_node_id != 0) return 0; // Root already exists

    // Create root node
    devicetree_node_t* root = &nodes[0];
    strncpy(root->name, name, sizeof(root->name) - 1);
    strncpy(root->compatible, compatible, sizeof(root->compatible) - 1);
    root->address = 0;
    root->size = 0;
    root->property_count = 0;
    root->child_count = 0;
    root->active = true;

    root_node_id = next_node_id++;
    return root_node_id;
}

// Create node
uint64_t devicetree_create_node(uint64_t parent_id, const char* name, const char* compatible, uint64_t address, uint64_t size) {
    devicetree_node_t* parent = devicetree_get_node(parent_id);
    if (!parent || !parent->active) return 0;

    // Check if parent has space for more children
    if (parent->child_count >= MAX_CHILDREN) return 0;

    // Find free node slot
    uint64_t node_id = 0;
    for (uint64_t i = 0; i < MAX_NODES; i++) {
        if (!nodes[i].active) {
            node_id = i;
            break;
        }
    }

    if (node_id == 0) return 0; // No free slots

    // Initialize node
    devicetree_node_t* node = &nodes[node_id];
    strncpy(node->name, name, sizeof(node->name) - 1);
    strncpy(node->compatible, compatible, sizeof(node->compatible) - 1);
    node->address = address;
    node->size = size;
    node->property_count = 0;
    node->child_count = 0;
    node->active = true;

    // Add to parent
    parent->children[parent->child_count++] = next_node_id;

    return next_node_id++;
}

// Add property
bool devicetree_add_property(uint64_t node_id, const char* name, const void* data, uint64_t size) {
    devicetree_node_t* node = devicetree_get_node(node_id);
    if (!node || !node->active) return false;

    // Check if node has space for more properties
    if (node->property_count >= MAX_PROPERTIES) return false;

    // Initialize property
    devicetree_property_t* property = &node->properties[node->property_count++];
    strncpy(property->name, name, sizeof(property->name) - 1);
    property->size = size;
    property->data = memory_allocate(size);
    if (!property->data) {
        node->property_count--;
        return false;
    }

    // Copy property data
    memcpy(property->data, data, size);
    property->active = true;

    return true;
}

// Get node
devicetree_node_t* devicetree_get_node(uint64_t node_id) {
    for (uint64_t i = 0; i < MAX_NODES; i++) {
        if (nodes[i].active && nodes[i].node_id == node_id) {
            return &nodes[i];
        }
    }
    return NULL;
}

// Get property
devicetree_property_t* devicetree_get_property(uint64_t node_id, const char* name) {
    devicetree_node_t* node = devicetree_get_node(node_id);
    if (!node || !node->active) return NULL;

    for (uint64_t i = 0; i < node->property_count; i++) {
        if (node->properties[i].active && strcmp(node->properties[i].name, name) == 0) {
            return &node->properties[i];
        }
    }

    return NULL;
}

// Get child node
uint64_t devicetree_get_child(uint64_t node_id, uint64_t index) {
    devicetree_node_t* node = devicetree_get_node(node_id);
    if (!node || !node->active || index >= node->child_count) return 0;

    return node->children[index];
}

// Get child count
uint64_t devicetree_get_child_count(uint64_t node_id) {
    devicetree_node_t* node = devicetree_get_node(node_id);
    return node ? node->child_count : 0;
}

// Get property count
uint64_t devicetree_get_property_count(uint64_t node_id) {
    devicetree_node_t* node = devicetree_get_node(node_id);
    return node ? node->property_count : 0;
}

// Get node by compatible
uint64_t devicetree_get_node_by_compatible(const char* compatible) {
    for (uint64_t i = 0; i < MAX_NODES; i++) {
        if (nodes[i].active && strcmp(nodes[i].compatible, compatible) == 0) {
            return nodes[i].node_id;
        }
    }
    return 0;
}

// Get node by address
uint64_t devicetree_get_node_by_address(uint64_t address) {
    for (uint64_t i = 0; i < MAX_NODES; i++) {
        if (nodes[i].active && nodes[i].address == address) {
            return nodes[i].node_id;
        }
    }
    return 0;
}

// Get node information
bool devicetree_get_node_info(uint64_t node_id, devicetree_node_info_t* info) {
    if (!info) return false;

    devicetree_node_t* node = devicetree_get_node(node_id);
    if (!node) return false;

    strncpy(info->name, node->name, sizeof(info->name) - 1);
    strncpy(info->compatible, node->compatible, sizeof(info->compatible) - 1);
    info->address = node->address;
    info->size = node->size;
    info->property_count = node->property_count;
    info->child_count = node->child_count;

    return true;
}

// Get property information
bool devicetree_get_property_info(uint64_t node_id, const char* name, devicetree_property_info_t* info) {
    if (!info) return false;

    devicetree_property_t* property = devicetree_get_property(node_id, name);
    if (!property) return false;

    strncpy(info->name, property->name, sizeof(info->name) - 1);
    info->size = property->size;
    info->data = property->data;

    return true;
}

// Get system statistics
void devicetree_get_system_stats(devicetree_system_stats_t* stats) {
    if (!stats) return;

    stats->total_nodes = 0;
    stats->active_nodes = 0;
    stats->total_properties = 0;
    stats->active_properties = 0;

    for (uint64_t i = 0; i < MAX_NODES; i++) {
        stats->total_nodes++;
        if (nodes[i].active) {
            stats->active_nodes++;
            stats->total_properties += nodes[i].property_count;
            for (uint64_t j = 0; j < nodes[i].property_count; j++) {
                if (nodes[i].properties[j].active) {
                    stats->active_properties++;
                }
            }
        }
    }
} 