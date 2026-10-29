global enter_usermode

; selector = (index << 3) | (ti << 2) | rpl
; ti -> table indicator
; rpl -> requested privilege level
enter_usermode:
    cli
    mov ax, (3 << 3) | (0 << 2) | 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push qword (3 << 3) | (0 << 2) | 3
    push rsi
    push qword 0x202
    push qword (4 << 3) | (0 << 2) | 3
    push rdi
    iretq
