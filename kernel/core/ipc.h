#ifndef IPC_H
#define IPC_H

#include <stdint.h>
#include <stdbool.h>

#define IPC_MAX_MSG_SIZE 256

typedef struct ipc_message {
    uint32_t sender_pid;
    uint32_t receiver_pid;
    uint8_t data[IPC_MAX_MSG_SIZE];
    uint32_t size;
} ipc_message_t;

void ipc_init(void);
bool ipc_send(uint32_t receiver_pid, const void* data, uint32_t size);
bool ipc_receive(uint32_t* sender_pid, void* buffer, uint32_t* size);

#endif // IPC_H 