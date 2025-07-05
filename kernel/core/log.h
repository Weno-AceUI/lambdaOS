#ifndef LOG_H
#define LOG_H

#include <stdint.h>
#include <stdarg.h>

void log_init(void);
void log_info(const char* fmt, ...);
void log_warn(const char* fmt, ...);
void log_error(const char* fmt, ...);
void log_panic(const char* fmt, ...);

#endif // LOG_H 