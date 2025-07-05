#include "security.h"
#include "memory.h"
#include "crypto.h"
#include "time.h"
#include <string.h>

#define MAX_USERS 1024
#define MAX_GROUPS 32
#define MAX_PERMISSIONS 32
#define MAX_SESSIONS 1024
#define MAX_ACL_ENTRIES 1024

// User structure
typedef struct {
    uint64_t user_id;
    char username[32];
    char password_hash[64];
    uint64_t group_id;
    bool active;
} user_t;

// Group structure
typedef struct {
    uint64_t group_id;
    char name[32];
    uint64_t permissions;
    bool active;
} group_t;

// Session structure
typedef struct {
    uint64_t session_id;
    uint64_t user_id;
    uint64_t start_time;
    uint64_t last_access;
    bool active;
} session_t;

// ACL entry structure
typedef struct {
    uint64_t entry_id;
    uint64_t resource_id;
    uint64_t user_id;
    uint64_t group_id;
    uint64_t permissions;
    bool active;
} acl_entry_t;

// Security service state
static user_t users[MAX_USERS];
static group_t groups[MAX_GROUPS];
static session_t sessions[MAX_SESSIONS];
static acl_entry_t acl_entries[MAX_ACL_ENTRIES];
static uint64_t next_user_id = 1;
static uint64_t next_group_id = 1;
static uint64_t next_session_id = 1;
static uint64_t next_acl_entry_id = 1;

// Initialize security service
void security_init(void) {
    memset(users, 0, sizeof(users));
    memset(groups, 0, sizeof(groups));
    memset(sessions, 0, sizeof(sessions));
    memset(acl_entries, 0, sizeof(acl_entries));
    next_user_id = 1;
    next_group_id = 1;
    next_session_id = 1;
    next_acl_entry_id = 1;

    // Create default groups
    security_create_group("root", PERMISSION_ALL);
    security_create_group("users", PERMISSION_READ | PERMISSION_WRITE);
    security_create_group("guests", PERMISSION_READ);
}

// Create user
uint64_t security_create_user(const char* username, const char* password, uint64_t group_id) {
    // Find free user slot
    uint64_t user_id = 0;
    for (uint64_t i = 0; i < MAX_USERS; i++) {
        if (!users[i].active) {
            user_id = i;
            break;
        }
    }

    if (user_id == 0) return 0; // No free slots

    // Initialize user
    user_t* user = &users[user_id];
    user->user_id = next_user_id++;
    strncpy(user->username, username, sizeof(user->username) - 1);
    
    // Hash password
    uint8_t hash[SHA256_HASH_SIZE];
    sha256_hash_string(password, hash);
    hash_to_hex(hash, SHA256_HASH_SIZE, user->password_hash);
    
    user->group_id = group_id;
    user->active = true;

    return user->user_id;
}

// Create group
uint64_t security_create_group(const char* name, uint64_t permissions) {
    // Find free group slot
    uint64_t group_id = 0;
    for (uint64_t i = 0; i < MAX_GROUPS; i++) {
        if (!groups[i].active) {
            group_id = i;
            break;
        }
    }

    if (group_id == 0) return 0; // No free slots

    // Initialize group
    group_t* group = &groups[group_id];
    group->group_id = next_group_id++;
    strncpy(group->name, name, sizeof(group->name) - 1);
    group->permissions = permissions;
    group->active = true;

    return group->group_id;
}

// Create session
uint64_t security_create_session(uint64_t user_id) {
    // Find free session slot
    uint64_t session_id = 0;
    for (uint64_t i = 0; i < MAX_SESSIONS; i++) {
        if (!sessions[i].active) {
            session_id = i;
            break;
        }
    }

    if (session_id == 0) return 0; // No free slots

    // Initialize session
    session_t* session = &sessions[session_id];
    session->session_id = next_session_id++;
    session->user_id = user_id;
    session->start_time = time_get_ms();
    session->last_access = session->start_time;
    session->active = true;

    return session->session_id;
}

// Add ACL entry
uint64_t security_add_acl_entry(uint64_t resource_id, uint64_t user_id, uint64_t group_id, uint64_t permissions) {
    // Find free ACL entry slot
    uint64_t entry_id = 0;
    for (uint64_t i = 0; i < MAX_ACL_ENTRIES; i++) {
        if (!acl_entries[i].active) {
            entry_id = i;
            break;
        }
    }

    if (entry_id == 0) return 0; // No free slots

    // Initialize ACL entry
    acl_entry_t* entry = &acl_entries[entry_id];
    entry->entry_id = next_acl_entry_id++;
    entry->resource_id = resource_id;
    entry->user_id = user_id;
    entry->group_id = group_id;
    entry->permissions = permissions;
    entry->active = true;

    return entry->entry_id;
}

// Authenticate user
bool security_authenticate(const char* username, const char* password) {
    // Find user
    for (uint64_t i = 0; i < MAX_USERS; i++) {
        if (users[i].active && strcmp(users[i].username, username) == 0) {
            // Hash input password
            uint8_t hash[SHA256_HASH_SIZE];
            char hash_hex[SHA256_HASH_SIZE * 2 + 1];
            sha256_hash_string(password, hash);
            hash_to_hex(hash, SHA256_HASH_SIZE, hash_hex);
            
            // Compare hashes
            return strcmp(users[i].password_hash, hash_hex) == 0;
        }
    }
    return false;
}

// Check permission
bool security_check_permission(uint64_t session_id, uint64_t resource_id, uint64_t permission) {
    session_t* session = security_get_session(session_id);
    if (!session || !session->active) return false;

    user_t* user = security_get_user(session->user_id);
    if (!user || !user->active) return false;

    // Check ACL entries
    for (uint64_t i = 0; i < MAX_ACL_ENTRIES; i++) {
        if (acl_entries[i].active && acl_entries[i].resource_id == resource_id) {
            // Check user-specific permissions
            if (acl_entries[i].user_id == user->user_id) {
                return (acl_entries[i].permissions & permission) == permission;
            }
            // Check group permissions
            if (acl_entries[i].group_id == user->group_id) {
                return (acl_entries[i].permissions & permission) == permission;
            }
        }
    }

    // Check group permissions
    group_t* group = security_get_group(user->group_id);
    if (group && group->active) {
        return (group->permissions & permission) == permission;
    }

    return false;
}

// Get user
user_t* security_get_user(uint64_t user_id) {
    for (uint64_t i = 0; i < MAX_USERS; i++) {
        if (users[i].active && users[i].user_id == user_id) {
            return &users[i];
        }
    }
    return NULL;
}

// Get group
group_t* security_get_group(uint64_t group_id) {
    for (uint64_t i = 0; i < MAX_GROUPS; i++) {
        if (groups[i].active && groups[i].group_id == group_id) {
            return &groups[i];
        }
    }
    return NULL;
}

// Get session
session_t* security_get_session(uint64_t session_id) {
    for (uint64_t i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].active && sessions[i].session_id == session_id) {
            return &sessions[i];
        }
    }
    return NULL;
}

// Get ACL entry
acl_entry_t* security_get_acl_entry(uint64_t entry_id) {
    for (uint64_t i = 0; i < MAX_ACL_ENTRIES; i++) {
        if (acl_entries[i].active && acl_entries[i].entry_id == entry_id) {
            return &acl_entries[i];
        }
    }
    return NULL;
}

// Get user count
uint64_t security_get_user_count(void) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < MAX_USERS; i++) {
        if (users[i].active) {
            count++;
        }
    }
    return count;
}

// Get group count
uint64_t security_get_group_count(void) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < MAX_GROUPS; i++) {
        if (groups[i].active) {
            count++;
        }
    }
    return count;
}

// Get session count
uint64_t security_get_session_count(void) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].active) {
            count++;
        }
    }
    return count;
}

// Get ACL entry count
uint64_t security_get_acl_entry_count(void) {
    uint64_t count = 0;
    for (uint64_t i = 0; i < MAX_ACL_ENTRIES; i++) {
        if (acl_entries[i].active) {
            count++;
        }
    }
    return count;
}

// Get user list
bool security_get_user_list(uint64_t* user_ids, uint64_t* count) {
    if (!user_ids || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_USERS && found < *count; i++) {
        if (users[i].active) {
            user_ids[found++] = users[i].user_id;
        }
    }

    *count = found;
    return true;
}

// Get group list
bool security_get_group_list(uint64_t* group_ids, uint64_t* count) {
    if (!group_ids || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_GROUPS && found < *count; i++) {
        if (groups[i].active) {
            group_ids[found++] = groups[i].group_id;
        }
    }

    *count = found;
    return true;
}

// Get session list
bool security_get_session_list(uint64_t* session_ids, uint64_t* count) {
    if (!session_ids || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_SESSIONS && found < *count; i++) {
        if (sessions[i].active) {
            session_ids[found++] = sessions[i].session_id;
        }
    }

    *count = found;
    return true;
}

// Get ACL entry list
bool security_get_acl_entry_list(uint64_t* entry_ids, uint64_t* count) {
    if (!entry_ids || !count) return false;

    uint64_t found = 0;
    for (uint64_t i = 0; i < MAX_ACL_ENTRIES && found < *count; i++) {
        if (acl_entries[i].active) {
            entry_ids[found++] = acl_entries[i].entry_id;
        }
    }

    *count = found;
    return true;
}

// Get user information
bool security_get_user_info(uint64_t user_id, user_info_t* info) {
    if (!info) return false;

    user_t* user = security_get_user(user_id);
    if (!user) return false;

    strncpy(info->username, user->username, sizeof(info->username) - 1);
    info->group_id = user->group_id;

    return true;
}

// Get group information
bool security_get_group_info(uint64_t group_id, group_info_t* info) {
    if (!info) return false;

    group_t* group = security_get_group(group_id);
    if (!group) return false;

    strncpy(info->name, group->name, sizeof(info->name) - 1);
    info->permissions = group->permissions;

    return true;
}

// Get session information
bool security_get_session_info(uint64_t session_id, session_info_t* info) {
    if (!info) return false;

    session_t* session = security_get_session(session_id);
    if (!session) return false;

    info->user_id = session->user_id;
    info->start_time = session->start_time;
    info->last_access = session->last_access;

    return true;
}

// Get ACL entry information
bool security_get_acl_entry_info(uint64_t entry_id, acl_entry_info_t* info) {
    if (!info) return false;

    acl_entry_t* entry = security_get_acl_entry(entry_id);
    if (!entry) return false;

    info->resource_id = entry->resource_id;
    info->user_id = entry->user_id;
    info->group_id = entry->group_id;
    info->permissions = entry->permissions;

    return true;
}

// Get system statistics
void security_get_system_stats(security_system_stats_t* stats) {
    if (!stats) return;

    stats->total_users = 0;
    stats->active_users = 0;
    stats->total_groups = 0;
    stats->active_groups = 0;
    stats->total_sessions = 0;
    stats->active_sessions = 0;
    stats->total_acl_entries = 0;
    stats->active_acl_entries = 0;

    for (uint64_t i = 0; i < MAX_USERS; i++) {
        stats->total_users++;
        if (users[i].active) {
            stats->active_users++;
        }
    }

    for (uint64_t i = 0; i < MAX_GROUPS; i++) {
        stats->total_groups++;
        if (groups[i].active) {
            stats->active_groups++;
        }
    }

    for (uint64_t i = 0; i < MAX_SESSIONS; i++) {
        stats->total_sessions++;
        if (sessions[i].active) {
            stats->active_sessions++;
        }
    }

    for (uint64_t i = 0; i < MAX_ACL_ENTRIES; i++) {
        stats->total_acl_entries++;
        if (acl_entries[i].active) {
            stats->active_acl_entries++;
        }
    }
} 