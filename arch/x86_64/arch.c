#include <boot/arch.h>
#include <gdt.h>

void arch_init() {
    init_gdt();
}
