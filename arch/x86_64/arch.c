#include <boot/arch.h>
#include <gdt.h>
#include <idt.h>

void arch_init() {
    init_gdt();
    init_idt();
}
