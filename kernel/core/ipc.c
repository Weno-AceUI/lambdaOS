#include "ipc.h"
#include "process.h"
#include "memory.h"
#include <string.h>

#define MAX_IPC_CHANNELS 1024
#define MAX_IPC_MESSAGES 1024
#define IPC_MESSAGE_SIZE 4096

// IPC message structure
typedef struct {
    uint64_t sender_pid;
    uint64_t receiver_pid;
    uint64_t message_id;
    uint64_t size;
    uint8_t data[IPC_MESSAGE_SIZE];
    bool delivered;
} ipc_message_t;

// IPC channel structure
typedef struct {
    uint64_t channel_id;
    uint64_t owner_pid;
    uint64_t message_count;
    ipc_message_t messages[MAX_IPC_MESSAGES];
    bool active;
} ipc_channel_t;

// IPC system state
static ipc_channel_t ipc_channels[MAX_IPC_CHANNELS];
static uint64_t next_channel_id = 1;
static uint64_t next_message_id = 1;

// Initialize IPC system
void ipc_init(void) {
    memset(ipc_channels, 0, sizeof(ipc_channels));
    next_channel_id = 1;
    next_message_id = 1;
}

// Create IPC channel
uint64_t ipc_create_channel(uint64_t owner_pid) {
    // Find free channel
    uint64_t channel_id = 0;
    for (uint64_t i = 0; i < MAX_IPC_CHANNELS; i++) {
        if (!ipc_channels[i].active) {
            channel_id = i;
            break;
        }
    }

    if (channel_id == 0) return 0; // No free channels

    // Initialize channel
    ipc_channel_t* channel = &ipc_channels[channel_id];
    channel->channel_id = next_channel_id++;
    channel->owner_pid = owner_pid;
    channel->message_count = 0;
    channel->active = true;

    return channel->channel_id;
}

// Destroy IPC channel
bool ipc_destroy_channel(uint64_t channel_id) {
    ipc_channel_t* channel = ipc_get_channel(channel_id);
    if (!channel || !channel->active) return false;

    channel->active = false;
    channel->message_count = 0;
    return true;
}

// Send message
bool ipc_send_message(uint64_t channel_id, uint64_t sender_pid, uint64_t receiver_pid, const void* data, uint64_t size) {
    ipc_channel_t* channel = ipc_get_channel(channel_id);
    if (!channel || !channel->active) return false;

    // Check message size
    if (size > IPC_MESSAGE_SIZE) return false;

    // Check if channel is full
    if (channel->message_count >= MAX_IPC_MESSAGES) return false;

    // Create message
    ipc_message_t* message = &channel->messages[channel->message_count++];
    message->sender_pid = sender_pid;
    message->receiver_pid = receiver_pid;
    message->message_id = next_message_id++;
    message->size = size;
    message->delivered = false;

    // Copy message data
    memcpy(message->data, data, size);

    return true;
}

// Receive message
bool ipc_receive_message(uint64_t channel_id, uint64_t receiver_pid, void* data, uint64_t* size) {
    ipc_channel_t* channel = ipc_get_channel(channel_id);
    if (!channel || !channel->active) return false;

    // Find undelivered message for receiver
    for (uint64_t i = 0; i < channel->message_count; i++) {
        ipc_message_t* message = &channel->messages[i];
        if (!message->delivered && message->receiver_pid == receiver_pid) {
            // Check buffer size
            if (*size < message->size) {
                *size = message->size;
                return false;
            }

            // Copy message data
            memcpy(data, message->data, message->size);
            *size = message->size;
            message->delivered = true;

            return true;
        }
    }

    return false;
}

// Get channel
ipc_channel_t* ipc_get_channel(uint64_t channel_id) {
    for (uint64_t i = 0; i < MAX_IPC_CHANNELS; i++) {
        if (ipc_channels[i].active && ipc_channels[i].channel_id == channel_id) {
            return &ipc_channels[i];
        }
    }
    return NULL;
}

// Get message count
uint64_t ipc_get_message_count(uint64_t channel_id) {
    ipc_channel_t* channel = ipc_get_channel(channel_id);
    return channel ? channel->message_count : 0;
}

// Get undelivered message count
uint64_t ipc_get_undelivered_count(uint64_t channel_id, uint64_t receiver_pid) {
    ipc_channel_t* channel = ipc_get_channel(channel_id);
    if (!channel) return 0;

    uint64_t count = 0;
    for (uint64_t i = 0; i < channel->message_count; i++) {
        if (!channel->messages[i].delivered && channel->messages[i].receiver_pid == receiver_pid) {
            count++;
        }
    }
    return count;
}

// Clear channel
bool ipc_clear_channel(uint64_t channel_id) {
    ipc_channel_t* channel = ipc_get_channel(channel_id);
    if (!channel) return false;

    channel->message_count = 0;
    return true;
}

// Get channel owner
uint64_t ipc_get_channel_owner(uint64_t channel_id) {
    ipc_channel_t* channel = ipc_get_channel(channel_id);
    return channel ? channel->owner_pid : 0;
}

// Check if channel exists
bool ipc_channel_exists(uint64_t channel_id) {
    return ipc_get_channel(channel_id) != NULL;
}

// Get channel statistics
void ipc_get_channel_stats(uint64_t channel_id, ipc_channel_stats_t* stats) {
    if (!stats) return;

    ipc_channel_t* channel = ipc_get_channel(channel_id);
    if (!channel) {
        stats->message_count = 0;
        stats->undelivered_count = 0;
        return;
    }

    stats->message_count = channel->message_count;
    stats->undelivered_count = 0;
    for (uint64_t i = 0; i < channel->message_count; i++) {
        if (!channel->messages[i].delivered) {
            stats->undelivered_count++;
        }
    }
}

// Get message information
bool ipc_get_message_info(uint64_t channel_id, uint64_t message_index, ipc_message_info_t* info) {
    if (!info) return false;

    ipc_channel_t* channel = ipc_get_channel(channel_id);
    if (!channel || message_index >= channel->message_count) return false;

    ipc_message_t* message = &channel->messages[message_index];
    info->sender_pid = message->sender_pid;
    info->receiver_pid = message->receiver_pid;
    info->message_id = message->message_id;
    info->size = message->size;
    info->delivered = message->delivered;

    return true;
}

// Get system statistics
void ipc_get_system_stats(ipc_system_stats_t* stats) {
    if (!stats) return;

    stats->total_channels = 0;
    stats->active_channels = 0;
    stats->total_messages = 0;
    stats->undelivered_messages = 0;

    for (uint64_t i = 0; i < MAX_IPC_CHANNELS; i++) {
        if (ipc_channels[i].active) {
            stats->active_channels++;
            stats->total_messages += ipc_channels[i].message_count;

            for (uint64_t j = 0; j < ipc_channels[i].message_count; j++) {
                if (!ipc_channels[i].messages[j].delivered) {
                    stats->undelivered_messages++;
                }
            }
        }
        stats->total_channels++;
    }
} 