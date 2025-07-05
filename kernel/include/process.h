#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Process states
typedef enum {
    PROCESS_STATE_NEW,
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} process_state_t;

// Process priority levels
typedef enum {
    PRIORITY_IDLE = 0,
    PRIORITY_LOW = 1,
    PRIORITY_NORMAL = 2,
    PRIORITY_HIGH = 3,
    PRIORITY_REALTIME = 4
} process_priority_t;

// Process control block (PCB)
typedef struct {
    uint64_t pid;                    // Process ID
    char name[32];                   // Process name
    process_state_t state;           // Current state
    process_priority_t priority;     // Priority level
    uint64_t stack_pointer;          // Stack pointer
    uint64_t program_counter;        // Program counter
    uint64_t memory_start;           // Start of process memory
    uint64_t memory_size;            // Size of process memory
    uint64_t parent_pid;             // Parent process ID
    uint64_t exit_code;              // Exit code when terminated
    uint64_t cpu_time;               // CPU time used
    uint64_t creation_time;          // Process creation timestamp
} process_control_block_t;

// Initialize process management
void process_init(void);

// Create a new process
int process_create(void (*entry)(void), size_t stack_size);

// Destroy a process
void process_destroy(int pid);

#endif // PROCESS_H 