#ifndef USERDATA_H
#define USERDATA_H

#include <stdint.h>
#include <stdbool.h>

// Contact information structure
typedef struct {
    char name[64];
    char phone[32];
    char email[128];
    bool favorite;
} contact_info_t;

// Message information structure
typedef struct {
    uint64_t contact_id;
    char content[1024];
    uint64_t timestamp;
    bool read;
} message_info_t;

// App preference information structure
typedef struct {
    char value[256];
} pref_info_t;

// System statistics structure
typedef struct {
    uint64_t total_contacts;
    uint64_t active_contacts;
    uint64_t total_messages;
    uint64_t active_messages;
    uint64_t total_prefs;
    uint64_t active_prefs;
} userdata_system_stats_t;

// Initialize user data service
void userdata_init(void);

// Add contact
uint64_t userdata_add_contact(const char* name, const char* phone, const char* email);

// Add message
uint64_t userdata_add_message(uint64_t contact_id, const char* content);

// Set app preference
uint64_t userdata_set_pref(const char* app_name, const char* key, const char* value);

// Get contact list
bool userdata_get_contact_list(uint64_t* contact_ids, uint64_t* count);

// Get message list
bool userdata_get_message_list(uint64_t contact_id, uint64_t* message_ids, uint64_t* count);

// Get app preference list
bool userdata_get_pref_list(const char* app_name, char** keys, uint64_t* count);

// Get contact information
bool userdata_get_contact_info(uint64_t contact_id, contact_info_t* info);

// Get message information
bool userdata_get_message_info(uint64_t message_id, message_info_t* info);

// Get app preference information
bool userdata_get_pref_info(const char* app_name, const char* key, pref_info_t* info);

// Get system statistics
void userdata_get_system_stats(userdata_system_stats_t* stats);

#endif // USERDATA_H 