#include "process.h"
#include "memory.h"
#include <stddef.h>
#include <string.h>

// Simple process control block structure
#define MAX_PROCESSES 32

typedef struct {
    int pid;
    void* stack;
    size_t stack_size;
    int used;
} simple_process_t;

static simple_process_t processes[MAX_PROCESSES];
static int next_pid = 1;

void process_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = 0;
        processes[i].stack = NULL;
        processes[i].stack_size = 0;
        processes[i].used = 0;
    }
    next_pid = 1;
}

int process_create(void (*entry)(void), size_t stack_size) {
    int slot = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (!processes[i].used) {
            slot = i;
            break;
        }
    }
    if (slot == -1) {
        return -1; // No free slots
    }
    void* stack = memory_alloc(stack_size);
    if (!stack) {
        return -1; // Memory allocation failed
    }
    processes[slot].pid = next_pid++;
    processes[slot].stack = stack;
    processes[slot].stack_size = stack_size;
    processes[slot].used = 1;
    // TODO: Set up stack frame for process entry if needed
    (void)entry; // Suppress unused warning
    return processes[slot].pid;
}

void process_destroy(int pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].used && processes[i].pid == pid) {
            if (processes[i].stack) {
                memory_free(processes[i].stack);
            }
            processes[i].pid = 0;
            processes[i].stack = NULL;
            processes[i].stack_size = 0;
            processes[i].used = 0;
            break;
        }
    }
} 