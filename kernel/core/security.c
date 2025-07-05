#include "security.h"

#define MAX_PROCESSES 64

static privilege_level_t privileges[MAX_PROCESSES];

void security_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        privileges[i] = PRIVILEGE_USER;
    }
}

bool security_set_privilege(uint32_t pid, privilege_level_t level) {
    if (pid >= MAX_PROCESSES) return false;
    privileges[pid] = level;
    return true;
}

privilege_level_t security_get_privilege(uint32_t pid) {
    if (pid >= MAX_PROCESSES) return PRIVILEGE_USER;
    return privileges[pid];
} 