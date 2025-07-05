// kernel/arch/aarch64/exception.c
#include <stdint.h>

void handle_syscall(uint64_t syscall_number, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    uint64_t result = syscall_handler(syscall_number, arg1, arg2, arg3);
    // Set return value in appropriate register (e.g., x0)
}
