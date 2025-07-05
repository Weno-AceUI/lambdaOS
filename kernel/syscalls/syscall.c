#include "syscall.h"
#include "drivers/console.h" // Hypothetical console driver
#include <stddef.h> // For size_t

uint64_t syscall_handler(uint64_t syscall_number, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    switch (syscall_number) {
        case SYSCALL_WRITE:
            console_write((const char *)arg1, (size_t)arg2);
            return 0;
        case SYSCALL_EXIT:
            // In real kernel: clean up and terminate the process
            return 0;
        default:
            return (uint64_t)-1; // Invalid syscall
    }
}

typedef uint64_t (*syscall_fn_t)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
uint64_t sys_write(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    // Example implementation using console_write
    console_write((const char *)arg1, (size_t)arg2);
    return 0;
}

uint64_t sys_read(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    // Stub implementation
    return 0;
}

static syscall_fn_t syscall_table[] = {
    sys_write,  // syscall 0
    sys_read,   // syscall 1
    // Add more here
};


