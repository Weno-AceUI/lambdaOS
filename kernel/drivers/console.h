#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

void console_clear();
void console_putc(char c);
void console_write(const char* str);
void console_write_hex(uint64_t value);
void console_write_dec(uint64_t value);

#endif // CONSOLE_H
