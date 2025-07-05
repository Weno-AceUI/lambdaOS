#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>
#include "process.h"

typedef enum {
    SCHED_RR,      // Round-robin
    SCHED_PRIORITY // Priority-based
} scheduler_policy_t;

void scheduler_init(scheduler_policy_t policy);
void scheduler_tick(void);
void scheduler_schedule(void);
void scheduler_set_policy(scheduler_policy_t policy);
process_control_block_t* scheduler_get_current(void);
void scheduler_yield(void);

#endif // SCHEDULER_H 