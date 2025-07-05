#include "power.h"
#include "driver.h"
#include <string.h>

#define MAX_POWER_STATES 8
#define MAX_POWER_EVENTS 32

// Power management state
static power_state_info_t power_states[MAX_POWER_STATES];

typedef struct {
    power_event_t event;
    char name[32];
    power_event_handler_t handler;
    void* context;
    bool active;
} power_event_info_t;

static power_event_info_t power_events[MAX_POWER_EVENTS];
static power_state_t current_state = POWER_STATE_ACTIVE;
static uint64_t next_event_id = 1;

// Initialize power management
void power_init(void) {
    memset(power_states, 0, sizeof(power_states));
    memset(power_events, 0, sizeof(power_events));
    current_state = POWER_STATE_ACTIVE;
    next_event_id = 1;

    // Register default power states
    power_register_state(POWER_STATE_ACTIVE, "Active", 100);
    power_register_state(POWER_STATE_IDLE, "Idle", 50);
    power_register_state(POWER_STATE_SLEEP, "Sleep", 10);
    power_register_state(POWER_STATE_HIBERNATE, "Hibernate", 1);
}

// Register power state
bool power_register_state(power_state_t state, const char* name, uint64_t power_usage) {
    // Find free state slot
    uint64_t index = 0;
    for (uint64_t i = 0; i < MAX_POWER_STATES; i++) {
        if (!power_states[i].active) {
            index = i;
            break;
        }
    }

    if (index == 0) return false; // No free slots

    // Initialize state
    power_state_info_t* state_info = &power_states[index];
    state_info->state = state;
    strncpy(state_info->name, name, sizeof(state_info->name) - 1);
    state_info->power_usage = power_usage;
    state_info->active = true;

    return true;
}

// Set power state
bool power_set_state(power_state_t state) {
    // Find state
    power_state_info_t* state_info = NULL;
    for (uint64_t i = 0; i < MAX_POWER_STATES; i++) {
        if (power_states[i].active && power_states[i].state == state) {
            state_info = &power_states[i];
            break;
        }
    }

    if (!state_info) return false;

    // Notify power state change
    power_event_t event = {
        .type = POWER_EVENT_STATE_CHANGE,
        .state = state,
        .timestamp = 0 // TODO: Get system time
    };
    power_notify_event(&event);

    // Update current state
    current_state = state;

    return true;
}

// Get current power state
power_state_t power_get_state(void) {
    return current_state;
}

// Register power event handler
uint64_t power_register_event_handler(power_event_t type, const char* name, power_event_handler_t handler, void* context) {
    // Find free event slot
    uint64_t index = 0;
    for (uint64_t i = 0; i < MAX_POWER_EVENTS; i++) {
        if (!power_events[i].active) {
            index = i;
            break;
        }
    }

    if (index == 0) return 0; // No free slots

    // Initialize event handler
    power_event_info_t* event_info = &power_events[index];
    event_info->event = type;
    strncpy(event_info->name, name, sizeof(event_info->name) - 1);
    event_info->handler = handler;
    event_info->context = context;
    event_info->active = true;

    return next_event_id++;
}

// Unregister power event handler
bool power_unregister_event_handler(uint64_t handler_id) {
    // Find handler
    for (uint64_t i = 0; i < MAX_POWER_EVENTS; i++) {
        if (power_events[i].active && power_events[i].event.id == handler_id) {
            power_events[i].active = false;
            return true;
        }
    }
    return false;
}

// Notify power event
void power_notify_event(const power_event_t* event) {
    if (!event) return;

    // Call registered handlers
    for (uint64_t i = 0; i < MAX_POWER_EVENTS; i++) {
        if (power_events[i].active && power_events[i].event.type == event->type) {
            power_events[i].handler(event, power_events[i].context);
        }
    }
}

// Get power state information
bool power_get_state_info(power_state_t state, power_state_info_t* info) {
    if (!info) return false;

    // Find state
    for (uint64_t i = 0; i < MAX_POWER_STATES; i++) {
        if (power_states[i].active && power_states[i].state == state) {
            memcpy(info, &power_states[i], sizeof(power_state_info_t));
            return true;
        }
    }

    return false;
}

// Get power usage
uint64_t power_get_usage(void) {
    // Find current state
    for (uint64_t i = 0; i < MAX_POWER_STATES; i++) {
        if (power_states[i].active && power_states[i].state == current_state) {
            return power_states[i].power_usage;
        }
    }

    return 0;
}

// Get power state count
uint64_t power_get_state_count(void) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < MAX_POWER_STATES; i++) {
        if (power_states[i].active) {
            count++;
        }
    }
    return count;
}

// Get power state list
bool power_get_state_list(power_state_t* states, uint64_t* count) {
    if (!states || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_POWER_STATES && found < *count; i++) {
        if (power_states[i].active) {
            states[found++] = power_states[i].state;
        }
    }

    *count = found;
    return true;
}

// Get event handler count
uint64_t power_get_handler_count(power_event_type_t type) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < MAX_POWER_EVENTS; i++) {
        if (power_events[i].active && power_events[i].event.type == type) {
            count++;
        }
    }
    return count;
}

// Get event handler list
bool power_get_handler_list(power_event_type_t type, uint64_t* handler_ids, uint64_t* count) {
    if (!handler_ids || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_POWER_EVENTS && found < *count; i++) {
        if (power_events[i].active && power_events[i].event.type == type) {
            handler_ids[found++] = power_events[i].event.id;
        }
    }

    *count = found;
    return true;
}

// Get system statistics
void power_get_system_stats(power_system_stats_t* stats) {
    if (!stats) return;

    stats->total_states = 0;
    stats->active_states = 0;
    stats->total_handlers = 0;
    stats->active_handlers = 0;

    for (uint64_t i = 0; i < MAX_POWER_STATES; i++) {
        stats->total_states++;
        if (power_states[i].active) {
            stats->active_states++;
        }
    }

    for (uint64_t i = 0; i < MAX_POWER_EVENTS; i++) {
        stats->total_handlers++;
        if (power_events[i].active) {
            stats->active_handlers++;
        }
    }
} 