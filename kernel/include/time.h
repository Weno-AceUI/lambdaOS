#ifndef TIME_H
#define TIME_H

#include <stdint.h>

// Get current system time in milliseconds since boot
uint64_t time_get_ms(void);

// Get current system time in microseconds since boot
uint64_t time_get_us(void);

// Get current system time in nanoseconds since boot
uint64_t time_get_ns(void);

// Get current system time in seconds since boot
uint64_t time_get_seconds(void);

// Initialize time system
void time_init(void);

#endif // TIME_H 