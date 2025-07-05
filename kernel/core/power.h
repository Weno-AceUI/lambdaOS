#ifndef POWER_H
#define POWER_H

#include <stdint.h>
#include <stdbool.h>

// Power states
typedef enum {
    POWER_STATE_ACTIVE,
    POWER_STATE_IDLE,
    POWER_STATE_SLEEP,
    POWER_STATE_HIBERNATE
} power_state_t;

// Power event types
typedef enum {
    POWER_EVENT_STATE_CHANGE,
    POWER_EVENT_WAKEUP,
    POWER_EVENT_SLEEP,
    POWER_EVENT_SHUTDOWN
} power_event_type_t;

// Power event structure
typedef struct {
    power_event_type_t type;
    power_state_t state;
    uint64_t timestamp;
    uint64_t id;
} power_event_t;

// Power event handler function type
typedef void (*power_event_handler_t)(const power_event_t* event, void* context);

// Power state info structure
typedef struct {
    power_state_t state;
    char name[32];
    uint64_t power_usage;
    bool active;
} power_state_info_t;

// Power system stats structure
typedef struct {
    uint64_t total_states;
    uint64_t active_states;
    uint64_t total_handlers;
    uint64_t active_handlers;
} power_system_stats_t;

// Function declarations
void power_init(void);
bool power_register_state(power_state_t state, const char* name, uint64_t power_usage);
bool power_set_state(power_state_t state);
power_state_t power_get_state(void);
uint64_t power_register_event_handler(power_event_t type, const char* name, power_event_handler_t handler, void* context);
bool power_unregister_event_handler(uint64_t handler_id);
void power_notify_event(const power_event_t* event);
bool power_get_state_info(power_state_t state, power_state_info_t* info);
uint64_t power_get_usage(void);
uint64_t power_get_state_count(void);
bool power_get_state_list(power_state_t* states, uint64_t* count);
uint64_t power_get_handler_count(power_event_type_t type);
bool power_get_handler_list(power_event_type_t type, uint64_t* handler_ids, uint64_t* count);
void power_get_system_stats(power_system_stats_t* stats);

#endif // POWER_H 