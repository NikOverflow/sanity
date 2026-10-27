#include <stdint.h>

#include <kernel/kmain.h>
#include <boot/arch.h>
#include <kernel/kprint.h>

struct framebuffer *framebuffer;

void kmain(struct boot_info *boot_info) {
    // We are already in long mode
    framebuffer = boot_info->framebuffer;

    arch_init();

    kprint("Hello, world!");
    kprint("\n");
    kprint("sphinx of black quartz, judge my vow!");
    kprint("\n");
    kprint("SPHINX OF BLACK QUARTZ, JUDGE MY VOW!");
    kprint("\n");
    kprint("!\"#$%&'()*+,-./0123456789:;<=>?@[\\]^_`{|}~");
    kprint("\nvoid kprint(char *string) {");
    kprint("\n");
    kprintf("The best private key is: %d\n", 4);

    __asm__ volatile("int $1");
    __asm__ volatile("int $3");
    __asm__ volatile("int $3");
    __asm__ volatile("int $7");
    for(uint16_t i = 0; i < 256; i++) {
        framebuffer->address[i] = 0xFF0000;
    }
    while(1) {}
}
