#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

// Initialize memory management
void memory_init(void);

// Allocate memory
void* memory_alloc(size_t size);

// Free memory
void memory_free(void* ptr);

#endif // MEMORY_H 