#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

// Memory allocation functions
void* memory_alloc(size_t size);
void memory_free(void* ptr);

// Memory initialization
void memory_init(void);

// Memory statistics
typedef struct {
    uint64_t total_memory;
    uint64_t used_memory;
    uint64_t free_memory;
    uint64_t allocation_count;
} memory_stats_t;

void memory_get_stats(memory_stats_t* stats);

#endif // MEMORY_H 