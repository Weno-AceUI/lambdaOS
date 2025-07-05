#include "memory.h"
#include <stdlib.h>

void memory_init(void) {
    // Initialize memory management system (if needed)
}

void* memory_alloc(size_t size) {
    return malloc(size);
}

void memory_free(void* ptr) {
    free(ptr);
} 