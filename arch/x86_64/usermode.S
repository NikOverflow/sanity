.global enter_usermode

# selector = (index << 3) | (ti << 2) | rpl
# ti -> table indicator
# rpl -> requested privilege level
enter_usermode:
    cli
    mov $((3 << 3) | (0 << 2) | 3), %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    pushq $((3 << 3) | (0 << 2) | 3)
    push %rsi
    pushq $0x202
    pushq $((4 << 3) | (0 << 2) | 3)
    push %rdi
    iretq
