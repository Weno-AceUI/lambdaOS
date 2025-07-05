#include "scheduler.h"
#include "process.h"
#include <stddef.h>

static scheduler_policy_t current_policy = SCHED_RR;
static process_control_block_t* current_process = NULL;

void scheduler_init(scheduler_policy_t policy) {
    current_policy = policy;
    current_process = NULL;
}

void scheduler_tick(void) {
    // For now, just a stub
}

void scheduler_schedule(void) {
    if (current_policy == SCHED_RR) {
        // Round-robin: switch to next process
        if (current_process) {
            // In a real implementation, you would switch to the next process in the ready queue
            // For now, just a stub
        }
    }
}

void scheduler_set_policy(scheduler_policy_t policy) {
    current_policy = policy;
}

process_control_block_t* scheduler_get_current(void) {
    return current_process;
}

void scheduler_yield(void) {
    // In a real implementation, you would switch to the next process
    // For now, just a stub
} 