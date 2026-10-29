#include <stdint.h>

#include <boot/boot_info.h>
#include <printf.h>

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

static const char *exceptions[] = {
    "Divide Error (#DE)",
    "Debug Exception (#DB)",
    "NMI Interrupt",
    "Breakpoint (#BP)",
    "Overflow (#OF)",
    "BOUND Range Exceeded (#BR)",
    "Invalid Opcode (#UD)",
    "Device Not Available (#NM)",
    "Double Fault (#DF)",
    "Coprocessor Segment Overrun",
    "Invalid TSS (#TS)",
    "Segment Not Present (#NP)",
    "Stack-Segment Fault (#SS)",
    "General Protection (#GP)",
    "Page Fault (#PF)",
    "Intel Reserved",
    "x87 FPU Floating-Point Error (#MF)",
    "Alignment Check (#AC)",
    "Machine Check (#MC)",
    "SIMD Floating-Point Exception (#XM)",
    "Virtualization Exception (#VE)",
    "Control Protection Exception (#CP)",
    "Intel Reserved", "Intel Reserved",
    "Intel Reserved", "Intel Reserved",
    "Intel Reserved", "Intel Reserved",
    "Intel Reserved", "Intel Reserved",
    "Intel Reserved", "Intel Reserved"
};


void interrupt_dispatch(struct interrupt_frame *interrupt_frame) {
    uint64_t vector = interrupt_frame->vector;
    printf("CPU EXCEPTION: ");
    if(vector < 32) {
        printf("%s", exceptions[vector]);
    } else {
        printf("User Defined");
    }
    printf(" (Vector: %d, Error Code: %d)\n", vector, interrupt_frame->error_code);
    uint64_t fault_addr = 0;
    switch(vector) {
        case 0:
        case 4:
        case 5:
        case 6:
        case 10:
        case 11:
        case 12:
        case 13:
            printf("Kernel Panic!");
            while(1) {
                __asm__ volatile("cli; hlt");
            }
            break;
        case 14:
            __asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));
            printf("Page Fault at Virtual Address: %lx\n", fault_addr);
        default:
            break;
    }
}
