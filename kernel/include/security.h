#ifndef SECURITY_H
#define SECURITY_H

#include <stdint.h>
#include <stdbool.h>

// Permission flags
#define PERMISSION_NONE    0x00000000
#define PERMISSION_READ    0x00000001
#define PERMISSION_WRITE   0x00000002
#define PERMISSION_EXECUTE 0x00000004
#define PERMISSION_DELETE  0x00000008
#define PERMISSION_ALL     0xFFFFFFFF

// User information structure
typedef struct {
    char username[32];
    uint64_t group_id;
} user_info_t;

// Group information structure
typedef struct {
    char name[32];
    uint64_t permissions;
} group_info_t;

// Session information structure
typedef struct {
    uint64_t user_id;
    uint64_t start_time;
    uint64_t last_access;
} session_info_t;

// System statistics structure
typedef struct {
    uint64_t total_users;
    uint64_t active_users;
    uint64_t total_groups;
    uint64_t active_groups;
    uint64_t total_sessions;
    uint64_t active_sessions;
} security_system_stats_t;

// Initialize security service
void security_init(void);

// Create user
uint64_t security_create_user(const char* username, const char* password, uint64_t group_id);

// Create group
uint64_t security_create_group(const char* name, uint64_t permissions);

// Create session
uint64_t security_create_session(uint64_t user_id);

// Authenticate user
bool security_authenticate(const char* username, const char* password);

// Check permission
bool security_check_permission(uint64_t session_id, uint64_t permission);

// Get user count
uint64_t security_get_user_count(void);

// Get group count
uint64_t security_get_group_count(void);

// Get session count
uint64_t security_get_session_count(void);

// Get user list
bool security_get_user_list(uint64_t* user_ids, uint64_t* count);

// Get group list
bool security_get_group_list(uint64_t* group_ids, uint64_t* count);

// Get session list
bool security_get_session_list(uint64_t* session_ids, uint64_t* count);

// Get user information
bool security_get_user_info(uint64_t user_id, user_info_t* info);

// Get group information
bool security_get_group_info(uint64_t group_id, group_info_t* info);

// Get session information
bool security_get_session_info(uint64_t session_id, session_info_t* info);

// Get system statistics
void security_get_system_stats(security_system_stats_t* stats);

#endif // SECURITY_H 