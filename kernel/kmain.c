#include <stdint.h>

#include <kernel/kmain.h>
#include <boot/arch.h>
#include <kernel/kprint.h>

void kmain(BootInfo *bootInfo) {
    // We are already in long mode
    arch_init();

    kprint("Hello, world!", bootInfo->framebuffer);
    kprint("\n", bootInfo->framebuffer);
    kprint("sphinx of black quartz, judge my vow!", bootInfo->framebuffer);
    kprint("\n", bootInfo->framebuffer);
    kprint("SPHINX OF BLACK QUARTZ, JUDGE MY VOW!", bootInfo->framebuffer);
    kprint("\n", bootInfo->framebuffer);
    kprint("!\"#$%&'()*+,-./0123456789:;<=>?@[\\]^_`{|}~", bootInfo->framebuffer);
    kprint("\nvoid kprint(char *string) {", bootInfo->framebuffer);

    for(uint16_t i = 0; i < 256; i++) {
        bootInfo->framebuffer->address[i] = 0xFF0000;
    }
    volatile int a = 5;
    volatile int b = 0;
    volatile int c = a / b;
    (void)c;
    while(1) {}
}
