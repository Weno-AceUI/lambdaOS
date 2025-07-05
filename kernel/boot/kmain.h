// File: /kernel/core/kmain.c
#include <stdint.h>

void kmain() {
    // For now, just hang
    while (1) {
        __asm__ volatile("wfe");
    }
}
