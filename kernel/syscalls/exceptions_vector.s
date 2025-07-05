svc_handler:
    // Save registers
    // Load syscall number from x8
    // Call syscall_handler in C
    // Restore registers
    eret
