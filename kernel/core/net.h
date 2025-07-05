#ifndef NET_H
#define NET_H

#include <stdint.h>
#include <stdbool.h>

#define NET_MAX_PACKET_SIZE 1500

typedef struct net_packet {
    uint8_t data[NET_MAX_PACKET_SIZE];
    uint32_t size;
} net_packet_t;

void net_init(void);
bool net_send(const void* data, uint32_t size);
int net_receive(void* buffer, uint32_t max_size);

#endif // NET_H 