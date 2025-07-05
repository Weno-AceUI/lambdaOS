#include "network.h"
#include "memory.h"
#include <string.h>

#define MAX_INTERFACES 16
#define MAX_SOCKETS 1024
#define MAX_PACKETS 1024
#define MAX_PACKET_SIZE 1500

// Network interface structure
typedef struct {
    char name[32];
    uint8_t mac_address[6];
    uint32_t ip_address;
    uint32_t netmask;
    uint32_t gateway;
    bool active;
} network_interface_t;

// Network packet structure
typedef struct {
    uint8_t data[MAX_PACKET_SIZE];
    uint64_t size;
    uint64_t timestamp;
    bool active;
} network_packet_t;

// Network socket structure
typedef struct {
    uint64_t socket_id;
    socket_type_t type;
    uint32_t local_address;
    uint16_t local_port;
    uint32_t remote_address;
    uint16_t remote_port;
    network_packet_t* receive_buffer;
    uint64_t receive_buffer_size;
    uint64_t receive_buffer_head;
    uint64_t receive_buffer_tail;
    bool active;
} network_socket_t;

// Network stack state
static network_interface_t interfaces[MAX_INTERFACES];
static network_socket_t sockets[MAX_SOCKETS];
static network_packet_t packets[MAX_PACKETS];
static uint64_t next_socket_id = 1;

// Initialize network stack
void network_init(void) {
    memset(interfaces, 0, sizeof(interfaces));
    memset(sockets, 0, sizeof(sockets));
    memset(packets, 0, sizeof(packets));
    next_socket_id = 1;
}

// Register network interface
bool network_register_interface(const char* name, const uint8_t* mac_address, uint32_t ip_address, uint32_t netmask, uint32_t gateway) {
    // Find free interface slot
    uint64_t index = 0;
    for (uint64_t i = 0; i < MAX_INTERFACES; i++) {
        if (!interfaces[i].active) {
            index = i;
            break;
        }
    }

    if (index == 0) return false; // No free slots

    // Initialize interface
    network_interface_t* interface = &interfaces[index];
    strncpy(interface->name, name, sizeof(interface->name) - 1);
    memcpy(interface->mac_address, mac_address, 6);
    interface->ip_address = ip_address;
    interface->netmask = netmask;
    interface->gateway = gateway;
    interface->active = true;

    return true;
}

// Create socket
uint64_t network_create_socket(socket_type_t type) {
    // Find free socket slot
    uint64_t socket_id = 0;
    for (uint64_t i = 0; i < MAX_SOCKETS; i++) {
        if (!sockets[i].active) {
            socket_id = i;
            break;
        }
    }

    if (socket_id == 0) return 0; // No free slots

    // Initialize socket
    network_socket_t* socket = &sockets[socket_id];
    socket->socket_id = next_socket_id++;
    socket->type = type;
    socket->local_address = 0;
    socket->local_port = 0;
    socket->remote_address = 0;
    socket->remote_port = 0;
    socket->receive_buffer = memory_allocate(MAX_PACKET_SIZE * 32); // 32 packet buffer
    if (!socket->receive_buffer) {
        return 0;
    }
    socket->receive_buffer_size = MAX_PACKET_SIZE * 32;
    socket->receive_buffer_head = 0;
    socket->receive_buffer_tail = 0;
    socket->active = true;

    return socket->socket_id;
}

// Bind socket
bool network_bind_socket(uint64_t socket_id, uint32_t address, uint16_t port) {
    network_socket_t* socket = network_get_socket(socket_id);
    if (!socket || !socket->active) return false;

    socket->local_address = address;
    socket->local_port = port;
    return true;
}

// Connect socket
bool network_connect_socket(uint64_t socket_id, uint32_t address, uint16_t port) {
    network_socket_t* socket = network_get_socket(socket_id);
    if (!socket || !socket->active) return false;

    socket->remote_address = address;
    socket->remote_port = port;
    return true;
}

// Send data
bool network_send(uint64_t socket_id, const void* data, uint64_t size) {
    network_socket_t* socket = network_get_socket(socket_id);
    if (!socket || !socket->active) return false;

    // Find free packet slot
    uint64_t packet_id = 0;
    for (uint64_t i = 0; i < MAX_PACKETS; i++) {
        if (!packets[i].active) {
            packet_id = i;
            break;
        }
    }

    if (packet_id == 0) return false; // No free slots

    // Create packet
    network_packet_t* packet = &packets[packet_id];
    if (size > MAX_PACKET_SIZE) return false;
    memcpy(packet->data, data, size);
    packet->size = size;
    packet->timestamp = 0; // TODO: Get system time
    packet->active = true;

    // TODO: Send packet through network interface
    return true;
}

// Receive data
bool network_receive(uint64_t socket_id, void* data, uint64_t* size) {
    network_socket_t* socket = network_get_socket(socket_id);
    if (!socket || !socket->active || !data || !size) return false;

    // Check if buffer is empty
    if (socket->receive_buffer_head == socket->receive_buffer_tail) return false;

    // Get packet from buffer
    network_packet_t* packet = (network_packet_t*)(socket->receive_buffer + socket->receive_buffer_head);
    if (*size < packet->size) {
        *size = packet->size;
        return false;
    }

    // Copy data
    memcpy(data, packet->data, packet->size);
    *size = packet->size;

    // Update buffer
    socket->receive_buffer_head = (socket->receive_buffer_head + sizeof(network_packet_t)) % socket->receive_buffer_size;

    return true;
}

// Get socket
network_socket_t* network_get_socket(uint64_t socket_id) {
    for (uint64_t i = 0; i < MAX_SOCKETS; i++) {
        if (sockets[i].active && sockets[i].socket_id == socket_id) {
            return &sockets[i];
        }
    }
    return NULL;
}

// Get interface
network_interface_t* network_get_interface(const char* name) {
    for (uint64_t i = 0; i < MAX_INTERFACES; i++) {
        if (interfaces[i].active && strcmp(interfaces[i].name, name) == 0) {
            return &interfaces[i];
        }
    }
    return NULL;
}

// Get interface by address
network_interface_t* network_get_interface_by_address(uint32_t address) {
    for (uint64_t i = 0; i < MAX_INTERFACES; i++) {
        if (interfaces[i].active && interfaces[i].ip_address == address) {
            return &interfaces[i];
        }
    }
    return NULL;
}

// Get interface count
uint64_t network_get_interface_count(void) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < MAX_INTERFACES; i++) {
        if (interfaces[i].active) {
            count++;
        }
    }
    return count;
}

// Get interface list
bool network_get_interface_list(char** names, uint64_t* count) {
    if (!names || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_INTERFACES && found < *count; i++) {
        if (interfaces[i].active) {
            names[found++] = interfaces[i].name;
        }
    }

    *count = found;
    return true;
}

// Get socket count
uint64_t network_get_socket_count(void) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < MAX_SOCKETS; i++) {
        if (sockets[i].active) {
            count++;
        }
    }
    return count;
}

// Get socket list
bool network_get_socket_list(uint64_t* socket_ids, uint64_t* count) {
    if (!socket_ids || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_SOCKETS && found < *count; i++) {
        if (sockets[i].active) {
            socket_ids[found++] = sockets[i].socket_id;
        }
    }

    *count = found;
    return true;
}

// Get interface information
bool network_get_interface_info(const char* name, network_interface_info_t* info) {
    if (!info) return false;

    network_interface_t* interface = network_get_interface(name);
    if (!interface) return false;

    strncpy(info->name, interface->name, sizeof(info->name) - 1);
    memcpy(info->mac_address, interface->mac_address, 6);
    info->ip_address = interface->ip_address;
    info->netmask = interface->netmask;
    info->gateway = interface->gateway;

    return true;
}

// Get socket information
bool network_get_socket_info(uint64_t socket_id, network_socket_info_t* info) {
    if (!info) return false;

    network_socket_t* socket = network_get_socket(socket_id);
    if (!socket) return false;

    info->socket_id = socket->socket_id;
    info->type = socket->type;
    info->local_address = socket->local_address;
    info->local_port = socket->local_port;
    info->remote_address = socket->remote_address;
    info->remote_port = socket->remote_port;

    return true;
}

// Get system statistics
void network_get_system_stats(network_system_stats_t* stats) {
    if (!stats) return;

    stats->total_interfaces = 0;
    stats->active_interfaces = 0;
    stats->total_sockets = 0;
    stats->active_sockets = 0;
    stats->total_packets = 0;
    stats->active_packets = 0;

    for (uint64_t i = 0; i < MAX_INTERFACES; i++) {
        stats->total_interfaces++;
        if (interfaces[i].active) {
            stats->active_interfaces++;
        }
    }

    for (uint64_t i = 0; i < MAX_SOCKETS; i++) {
        stats->total_sockets++;
        if (sockets[i].active) {
            stats->active_sockets++;
        }
    }

    for (uint64_t i = 0; i < MAX_PACKETS; i++) {
        stats->total_packets++;
        if (packets[i].active) {
            stats->active_packets++;
        }
    }
} 