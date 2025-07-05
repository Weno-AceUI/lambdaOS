// File: /kernel/boot/boot.S
.section .text
.global _start

_start:
    // Set up the stack pointer (simple hardcoded for now)
    ldr x0, =_stack_top
    mov sp, x0

    // Call the main C kernel function
    bl kmain

    // Infinite loop if kmain returns
1:  wfe
    b 1b

.section .bss
.space 4096
_stack_top:
