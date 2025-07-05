#include "time.h"
#include "arch/time.h"

// Initialize time system
void time_init(void) {
    // Initialize hardware timer
    arch_time_init();
}

// Get current system time in milliseconds since boot
uint64_t time_get_ms(void) {
    return arch_time_get_ms();
}

// Get current system time in microseconds since boot
uint64_t time_get_us(void) {
    return arch_time_get_us();
}

// Get current system time in nanoseconds since boot
uint64_t time_get_ns(void) {
    return arch_time_get_ns();
}

// Get current system time in seconds since boot
uint64_t time_get_seconds(void) {
    return time_get_ms() / 1000;
} 