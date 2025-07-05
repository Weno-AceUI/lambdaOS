#ifndef WEBCPP_THREADING_H
#define WEBCPP_THREADING_H

#include <stdint.h>
#include <stdbool.h>

// Thread Priority
typedef enum {
    THREAD_PRIORITY_LOW,
    THREAD_PRIORITY_NORMAL,
    THREAD_PRIORITY_HIGH
} thread_priority_t;

// Thread Handle
typedef struct {
    uint32_t id;
    void* handle;
    thread_priority_t priority;
    bool running;
} thread_handle_t;

// Thread Function
typedef void (*thread_function_t)(void* arg);

// Initialize threading subsystem
bool threading_init(void);

// Create new thread
thread_handle_t* threading_create_thread(thread_function_t function, void* arg, thread_priority_t priority);

// Set thread priority
bool threading_set_priority(thread_handle_t* thread, thread_priority_t priority);

// Wait for thread completion
bool threading_join(thread_handle_t* thread);

// Terminate thread
bool threading_terminate(thread_handle_t* thread);

// Get current thread ID
uint32_t threading_get_current_id(void);

// Cleanup threading resources
void threading_cleanup(void);

#endif // WEBCPP_THREADING_H 