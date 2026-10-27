#include <stdint.h>

#include <boot/boot_info.h>
#include <kernel/kprint.h>

struct interrupt_frame {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t vector;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed));

void interrupt_dispatch(struct interrupt_frame *interrupt_frame) {
    kprintf("Interrupt occured: %d\n", interrupt_frame->vector);
    switch(interrupt_frame->vector) {
        case 0:
            while(1) {
                __asm__ volatile("cli; hlt");
            }
            break;
        default:
            break;
    }
}
