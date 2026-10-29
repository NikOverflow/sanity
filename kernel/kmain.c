#include <stdint.h>

#include <kernel/kmain.h>
#include <boot/arch.h>
#include <kernel/mm/pmm.h>
#include <printf.h>

struct framebuffer *framebuffer;

extern void enter_usermode(void *entry, uint64_t rsp);

void kmain(struct boot_info *boot_info) {
    // We are already in long mode
    framebuffer = boot_info->framebuffer;

    arch_init();
    init_pmm(boot_info->memory_map);

    uint64_t page = alloc_page();
    printf("Allocated page 1 at 0x%lx\n", page);
    uint64_t page2 = alloc_page();
    printf("Allocated page 2 at 0x%lx\n", page2);
    free_page(page2);
    printf("Freed page 2\n");
    free_page(page);
    printf("Freed page 1\n");
    page = alloc_page();
    printf("Allocated page 1 at 0x%lx\n", page);
    volatile uint64_t *ptr = (volatile uint64_t *)(page + boot_info->memory_map->offset);
    *ptr = 0xDEADBEEF;
    printf("Wrote value: 0x%x at 0x%lx successfully\n", *ptr, page);

    printf("Hello, world!\n");
    printf("sphinx of black quartz, judge my vow!\n");
    printf("SPHINX OF BLACK QUARTZ, JUDGE MY VOW!\n");
    printf("!\"#$%&'()*+,-./0123456789:;<=>?@[\\]^_`{|}~\n");
    printf("void kprint(char *string) {\n");
    printf("The best private key is: %d\n", 4);

    __asm__ volatile("int $1");
    __asm__ volatile("int $3");
    __asm__ volatile("int $3");
    __asm__ volatile("int $7");
    while(1) {}
}
