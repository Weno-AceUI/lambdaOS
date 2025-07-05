#include "userdata.h"
#include "../include/memory.h"
#include <string.h>

#define MAX_CONTACTS 1024
#define MAX_MESSAGES 4096
#define MAX_APP_PREFS 256

// Contact structure
typedef struct {
    uint64_t contact_id;
    char name[64];
    char phone[32];
    char email[128];
    bool favorite;
    bool active;
} contact_t;

// Message structure
typedef struct {
    uint64_t message_id;
    uint64_t contact_id;
    char content[1024];
    uint64_t timestamp;
    bool read;
    bool active;
} message_t;

// App preference structure
typedef struct {
    uint64_t pref_id;
    char app_name[32];
    char key[64];
    char value[256];
    bool active;
} app_pref_t;

// User data service state
static contact_t contacts[MAX_CONTACTS];
static message_t messages[MAX_MESSAGES];
static app_pref_t app_prefs[MAX_APP_PREFS];
static uint64_t next_contact_id = 1;
static uint64_t next_message_id = 1;
static uint64_t next_pref_id = 1;

// Initialize user data service
void userdata_init(void) {
    memset(contacts, 0, sizeof(contacts));
    memset(messages, 0, sizeof(messages));
    memset(app_prefs, 0, sizeof(app_prefs));
    next_contact_id = 1;
    next_message_id = 1;
    next_pref_id = 1;
}

// Add contact
uint64_t userdata_add_contact(const char* name, const char* phone, const char* email) {
    // Find free contact slot
    uint64_t contact_id = 0;
    for (uint64_t i = 0; i < MAX_CONTACTS; i++) {
        if (!contacts[i].active) {
            contact_id = i;
            break;
        }
    }

    if (contact_id == 0) return 0; // No free slots

    // Initialize contact
    contact_t* contact = &contacts[contact_id];
    contact->contact_id = next_contact_id++;
    strncpy(contact->name, name, sizeof(contact->name) - 1);
    strncpy(contact->phone, phone, sizeof(contact->phone) - 1);
    strncpy(contact->email, email, sizeof(contact->email) - 1);
    contact->favorite = false;
    contact->active = true;

    return contact->contact_id;
}

// Add message
uint64_t userdata_add_message(uint64_t contact_id, const char* content) {
    // Find free message slot
    uint64_t message_id = 0;
    for (uint64_t i = 0; i < MAX_MESSAGES; i++) {
        if (!messages[i].active) {
            message_id = i;
            break;
        }
    }

    if (message_id == 0) return 0; // No free slots

    // Initialize message
    message_t* message = &messages[message_id];
    message->message_id = next_message_id++;
    message->contact_id = contact_id;
    strncpy(message->content, content, sizeof(message->content) - 1);
    message->timestamp = time_get_ms();
    message->read = false;
    message->active = true;

    return message->message_id;
}

// Set app preference
uint64_t userdata_set_pref(const char* app_name, const char* key, const char* value) {
    // Find existing preference
    for (uint64_t i = 0; i < MAX_APP_PREFS; i++) {
        if (app_prefs[i].active && 
            strcmp(app_prefs[i].app_name, app_name) == 0 && 
            strcmp(app_prefs[i].key, key) == 0) {
            strncpy(app_prefs[i].value, value, sizeof(app_prefs[i].value) - 1);
            return app_prefs[i].pref_id;
        }
    }

    // Find free preference slot
    uint64_t pref_id = 0;
    for (uint64_t i = 0; i < MAX_APP_PREFS; i++) {
        if (!app_prefs[i].active) {
            pref_id = i;
            break;
        }
    }

    if (pref_id == 0) return 0; // No free slots

    // Initialize preference
    app_pref_t* pref = &app_prefs[pref_id];
    pref->pref_id = next_pref_id++;
    strncpy(pref->app_name, app_name, sizeof(pref->app_name) - 1);
    strncpy(pref->key, key, sizeof(pref->key) - 1);
    strncpy(pref->value, value, sizeof(pref->value) - 1);
    pref->active = true;

    return pref->pref_id;
}

// Get contact
contact_t* userdata_get_contact(uint64_t contact_id) {
    for (uint64_t i = 0; i < MAX_CONTACTS; i++) {
        if (contacts[i].active && contacts[i].contact_id == contact_id) {
            return &contacts[i];
        }
    }
    return NULL;
}

// Get message
message_t* userdata_get_message(uint64_t message_id) {
    for (uint64_t i = 0; i < MAX_MESSAGES; i++) {
        if (messages[i].active && messages[i].message_id == message_id) {
            return &messages[i];
        }
    }
    return NULL;
}

// Get app preference
app_pref_t* userdata_get_pref(const char* app_name, const char* key) {
    for (uint64_t i = 0; i < MAX_APP_PREFS; i++) {
        if (app_prefs[i].active && 
            strcmp(app_prefs[i].app_name, app_name) == 0 && 
            strcmp(app_prefs[i].key, key) == 0) {
            return &app_prefs[i];
        }
    }
    return NULL;
}

// Get contact list
bool userdata_get_contact_list(uint64_t* contact_ids, uint64_t* count) {
    if (!contact_ids || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_CONTACTS && found < *count; i++) {
        if (contacts[i].active) {
            contact_ids[found++] = contacts[i].contact_id;
        }
    }

    *count = found;
    return true;
}

// Get message list
bool userdata_get_message_list(uint64_t contact_id, uint64_t* message_ids, uint64_t* count) {
    if (!message_ids || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_MESSAGES && found < *count; i++) {
        if (messages[i].active && messages[i].contact_id == contact_id) {
            message_ids[found++] = messages[i].message_id;
        }
    }

    *count = found;
    return true;
}

// Get app preference list
bool userdata_get_pref_list(const char* app_name, char** keys, uint64_t* count) {
    if (!keys || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_APP_PREFS && found < *count; i++) {
        if (app_prefs[i].active && strcmp(app_prefs[i].app_name, app_name) == 0) {
            keys[found++] = app_prefs[i].key;
        }
    }

    *count = found;
    return true;
}

// Get contact information
bool userdata_get_contact_info(uint64_t contact_id, contact_info_t* info) {
    if (!info) return false;

    contact_t* contact = userdata_get_contact(contact_id);
    if (!contact) return false;

    strncpy(info->name, contact->name, sizeof(info->name) - 1);
    strncpy(info->phone, contact->phone, sizeof(info->phone) - 1);
    strncpy(info->email, contact->email, sizeof(info->email) - 1);
    info->favorite = contact->favorite;

    return true;
}

// Get message information
bool userdata_get_message_info(uint64_t message_id, message_info_t* info) {
    if (!info) return false;

    message_t* message = userdata_get_message(message_id);
    if (!message) return false;

    info->contact_id = message->contact_id;
    strncpy(info->content, message->content, sizeof(info->content) - 1);
    info->timestamp = message->timestamp;
    info->read = message->read;

    return true;
}

// Get app preference information
bool userdata_get_pref_info(const char* app_name, const char* key, pref_info_t* info) {
    if (!info) return false;

    app_pref_t* pref = userdata_get_pref(app_name, key);
    if (!pref) return false;

    strncpy(info->value, pref->value, sizeof(info->value) - 1);

    return true;
}

// Get system statistics
void userdata_get_system_stats(userdata_system_stats_t* stats) {
    if (!stats) return;

    stats->total_contacts = 0;
    stats->active_contacts = 0;
    stats->total_messages = 0;
    stats->active_messages = 0;
    stats->total_prefs = 0;
    stats->active_prefs = 0;

    for (uint64_t i = 0; i < MAX_CONTACTS; i++) {
        stats->total_contacts++;
        if (contacts[i].active) {
            stats->active_contacts++;
        }
    }

    for (uint64_t i = 0; i < MAX_MESSAGES; i++) {
        stats->total_messages++;
        if (messages[i].active) {
            stats->active_messages++;
        }
    }

    for (uint64_t i = 0; i < MAX_APP_PREFS; i++) {
        stats->total_prefs++;
        if (app_prefs[i].active) {
            stats->active_prefs++;
        }
    }
} 