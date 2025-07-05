/* File: /kernel/boot/linker.ld */
ENTRY(_start)

SECTIONS {
    . = 0x80000; /* Typical load address for ARM64 kernels */

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss COMMON)
    }
}
