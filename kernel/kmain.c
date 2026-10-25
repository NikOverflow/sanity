#include <kernel/kmain.h>
#include <boot/arch.h>

void kmain(BootInfo *bootInfo) {
    // We are already in long mode
    arch_init();
    for(int i = 0; i < 255; i++) {
        bootInfo->address[i] = 0xFF0000;
    }
    volatile int a = 5;
    volatile int b = 0;
    volatile int c = a / b;
    (void)c;
    while(1) {}
}
