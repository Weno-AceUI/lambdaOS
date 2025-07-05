#ifndef OPENACE_SYSCALL_H
#define OPENACE_SYSCALL_H

#include <stdint.h>

#define SYSCALL_WRITE  0
#define SYSCALL_EXIT   1
// Add more syscall numbers here

uint64_t syscall_handler(uint64_t syscall_number, uint64_t arg1, uint64_t arg2, uint64_t arg3);

#endif
