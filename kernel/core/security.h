#ifndef SECURITY_H
#define SECURITY_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    PRIVILEGE_USER,
    PRIVILEGE_KERNEL
} privilege_level_t;

void security_init(void);
bool security_set_privilege(uint32_t pid, privilege_level_t level);
privilege_level_t security_get_privilege(uint32_t pid);

#endif // SECURITY_H 