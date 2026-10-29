#include <stdint.h>

#include <idt.h>

#define IDT_TYPE_INTERRUPT 0x0E
#define IDT_TYPE_TRAP      0x0F

#define IDT_FLAG_RING0   0b000
#define IDT_FLAG_RING1   0b001
#define IDT_FLAG_RING2   0b010
#define IDT_FLAG_RING3   0b011
#define IDT_FLAG_PRESENT 0b100

struct idt_entry {
    uint16_t base_low;
    uint16_t segment_selector;
    uint8_t ist: 3;
    uint8_t reserved: 5;
    uint8_t type: 4;
    uint8_t reserved2: 1;
    uint8_t flags: 3;
    uint16_t base_middle;
    uint32_t base_high;
    uint32_t reserved3;
} __attribute__((packed));

struct idt_pointer {
    uint16_t limit;
    struct idt_entry *base;
} __attribute__((packed));

extern void load_idt(struct idt_pointer *idtr_pointer);

struct idt_entry idt_entries[256];
struct idt_pointer idtr;

extern void *isr_stub_table[256];

void set_idt_gate(uint8_t interrupt, void *base, uint16_t segment_selector, uint8_t ist, uint8_t type, uint8_t flags) {
    uintptr_t address = (uintptr_t) base;
    idt_entries[interrupt].base_low = address & 0xFFFF;
    idt_entries[interrupt].segment_selector = segment_selector;
    idt_entries[interrupt].ist = ist;
    idt_entries[interrupt].reserved = 0x00;
    idt_entries[interrupt].type = type;
    idt_entries[interrupt].reserved2 = 0x00;
    idt_entries[interrupt].flags = flags;
    idt_entries[interrupt].base_middle = (address >> 16) & 0xFFFF;
    idt_entries[interrupt].base_high = (address >> 32) & 0xFFFFFFFF;
    idt_entries[interrupt].reserved3 = 0x00;
}

void init_idt() {
    idtr.limit = sizeof(idt_entries) - 1;
    idtr.base = idt_entries;

    for(uint16_t i = 0; i < 256; i++) {
        set_idt_gate(i, isr_stub_table[i], 0x08, 0, IDT_TYPE_INTERRUPT, IDT_FLAG_PRESENT | IDT_FLAG_RING0);
    }
    load_idt(&idtr);
}
