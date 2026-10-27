#include <stdint.h>

#include <gdt.h>

#define GDT_ACCESS_ACCESSED                0b00000001
#define GDT_ACCESS_CODE_READABLE           0b00000010
#define GDT_ACCESS_DATA_WRITABLE           0b00000010
#define GDT_ACCESS_CODE_CONFORMING         0b00000100
#define GDT_ACCESS_DATA_DIRECTION_NORMAL   0b00000000
#define GDT_ACCESS_DATA_DIRECTION_REVERSED 0b00000100
#define GDT_ACCESS_CODE_SEGMENT            0b00011000
#define GDT_ACCESS_DATA_SEGMENT            0b00010000
#define GDT_ACCESS_RING0                   0b00000000
#define GDT_ACCESS_RING1                   0b00100000
#define GDT_ACCESS_RING2                   0b01000000
#define GDT_ACCESS_RING3                   0b01100000
#define GDT_ACCESS_PRESENT                 0b10000000

#define GDT_FLAG_AVAILABLE      0b0001
#define GDT_FLAG_LONG_MODE      0b0010
#define GDT_FLAG_PROTECTED_MODE 0b0100
#define GDT_FLAG_REAL_MODE      0b0000
#define GDT_FLAG_GRANULARITY_1B 0b0000
#define GDT_FLAG_GRANULARITY_4K 0b1000

struct gdt_entry {
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMiddle;
    uint8_t access;
    uint8_t limitHigh: 4;
    uint8_t flags: 4;
    uint8_t baseHigh;
} __attribute__((packed));

struct gdt_pointer {
    uint16_t limit;
    struct gdt_entry *base;
} __attribute__((packed));

struct gdt_entry gdt_entries[3];
struct gdt_pointer gdtr;

void set_gdt_gate(uint8_t number, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt_entries[number].limitLow = limit & 0xFFFF;
    gdt_entries[number].baseLow = base & 0xFFFF;
    gdt_entries[number].baseMiddle = (base >> 16) & 0xFF;
    gdt_entries[number].access = access;
    gdt_entries[number].limitHigh = (limit >> 16) & 0xF;
    gdt_entries[number].flags = flags;
    gdt_entries[number].baseHigh = (base >> 24) & 0xFF;
}

extern void load_gdt(struct gdt_pointer *gdtr_pointer);

void init_gdt() {
    gdtr.limit = sizeof(gdt_entries) - 1;
    gdtr.base = gdt_entries;

    // null descriptor
    set_gdt_gate(0, 0x00, 0x00, 0, 0);
    // kernel code segment (0x08)
    set_gdt_gate(1, 0x00, 0xFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE, GDT_FLAG_LONG_MODE | GDT_FLAG_GRANULARITY_4K);
    // kernel data segment (0x10)
    set_gdt_gate(2, 0x00, 0xFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITABLE, GDT_FLAG_PROTECTED_MODE | GDT_FLAG_GRANULARITY_4K);

    load_gdt(&gdtr);
}
