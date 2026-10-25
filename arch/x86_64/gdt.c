#include <stdint.h>

#include <gdt.h>

#define GDT_ACCESS_ACCESSED                0b0'00'0'0001
#define GDT_ACCESS_CODE_READABLE           0b0'00'0'0010
#define GDT_ACCESS_DATA_WRITABLE           0b0'00'0'0010
#define GDT_ACCESS_CODE_CONFORMING         0b0'00'0'0100
#define GDT_ACCESS_DATA_DIRECTION_NORMAL   0b0'00'0'0000
#define GDT_ACCESS_DATA_DIRECTION_REVERSED 0b0'00'0'0100
#define GDT_ACCESS_CODE_SEGMENT            0b0'00'1'1000
#define GDT_ACCESS_DATA_SEGMENT            0b0'00'1'0000
#define GDT_ACCESS_RING0                   0b0'00'0'0000
#define GDT_ACCESS_RING1                   0b0'01'0'0000
#define GDT_ACCESS_RING2                   0b0'10'0'0000
#define GDT_ACCESS_RING3                   0b0'11'0'0000
#define GDT_ACCESS_PRESENT                 0b1'00'0'0000

#define GDT_FLAG_AVAILABLE      0b0'0'0'1
#define GDT_FLAG_LONG_MODE      0b0'0'1'0
#define GDT_FLAG_PROTECTED_MODE 0b0'1'0'0
#define GDT_FLAG_REAL_MODE      0b0'0'0'0
#define GDT_FLAG_GRANULARITY_1B 0b0'0'0'0
#define GDT_FLAG_GRANULARITY_4K 0b1'0'0'0

typedef struct {
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMiddle;
    uint8_t access;
    uint8_t limitHigh: 4;
    uint8_t flags: 4;
    uint8_t baseHigh;
} __attribute__((packed)) gdtEntry;

typedef struct {
    uint16_t limit;
    gdtEntry *base;
} __attribute__((packed)) gdtPointer;

gdtEntry gdtEntries[3];
gdtPointer gdtr;

void set_gdt_gate(uint8_t number, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdtEntries[number].limitLow = limit & 0xFFFF;
    gdtEntries[number].baseLow = base & 0xFFFF;
    gdtEntries[number].baseMiddle = (base >> 16) & 0xFF;
    gdtEntries[number].access = access;
    gdtEntries[number].limitHigh = (limit >> 16) & 0xF;
    gdtEntries[number].flags = flags;
    gdtEntries[number].baseHigh = (base >> 24) & 0xFF;
}

extern void load_gdt(gdtPointer *gdtrPointer);

void init_gdt() {
    gdtr.limit = sizeof(gdtEntries) - 1;
    gdtr.base = gdtEntries;

    // null descriptor
    set_gdt_gate(0, 0x00, 0x00, 0, 0);
    // kernel code segment (0x08)
    set_gdt_gate(1, 0x00, 0xFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE, GDT_FLAG_LONG_MODE | GDT_FLAG_GRANULARITY_4K);
    // kernel data segment (0x10)
    set_gdt_gate(2, 0x00, 0xFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITABLE, GDT_FLAG_PROTECTED_MODE | GDT_FLAG_GRANULARITY_4K);

    load_gdt(&gdtr);
}
