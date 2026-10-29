default rel

global load_gdt
global load_tss

load_gdt:
    lgdt [rdi]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    push 0x08
    lea rax, [.reload_cs]
    push rax
    retfq

.reload_cs:
    ret

load_tss:
    mov ax, di
    ltr ax
    ret
