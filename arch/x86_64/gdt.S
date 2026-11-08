.global load_gdt
.global load_tss

load_gdt:
    lgdt (%rdi)
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    push $0x08
    lea .reload_cs(%rip), %rax
    push %rax
    lretq

.reload_cs:
    ret

load_tss:
    mov %di, %ax
    ltr %ax
    ret
