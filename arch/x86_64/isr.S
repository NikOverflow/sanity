.global isr_stub_table

.extern interrupt_dispatch

.macro ISR int
isr\int:
    .if (\int == 8) || (\int > 9 && \int < 15) || (\int == 17) || (\int == 21)
    push $\int
    .else
    pushq $0
    pushq $\int
    .endif
    jmp interrupt_common
.endmacro

.macro isr_table_entry i
.quad isr\i
.endmacro

interrupt_common:
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rbp
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15
    mov %rsp, %rdi
    call interrupt_dispatch
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rbp
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
    add $16, %rsp
    iretq

.set i, 0
.altmacro
.rept 256
ISR %i
.set i, i + 1
.endr
.noaltmacro

isr_stub_table:
.set i, 0
.altmacro
.rept 256
isr_table_entry %i
.set i, i + 1
.endr
.noaltmacro
