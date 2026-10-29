#include <stdint.h>

#include <gdt.h>

#define GDT_ACCESS_ACCESSED                0b00000001
#define GDT_ACCESS_CODE_READABLE           0b00000010
#define GDT_ACCESS_DATA_WRITABLE           0b00000010
#define GDT_ACCESS_CODE_CONFORMING         0b00000100
#define GDT_ACCESS_DATA_DIRECTION_NORMAL   0b00000000
#define GDT_ACCESS_DATA_DIRECTION_REVERSED 0b00000100
#define GDT_ACCESS_SYSTEM_SEGMENT          0b00000000
#define GDT_ACCESS_CODE_SEGMENT            0b00011000
#define GDT_ACCESS_DATA_SEGMENT            0b00010000
#define GDT_ACCESS_RING0                   0b00000000
#define GDT_ACCESS_RING1                   0b00100000
#define GDT_ACCESS_RING2                   0b01000000
#define GDT_ACCESS_RING3                   0b01100000
#define GDT_ACCESS_PRESENT                 0b10000000

#define TSS_ACCESS_AVAILABLE 0b00001001

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
    uint8_t limitHigh : 4;
    uint8_t flags     : 4;
    uint8_t baseHigh;
} __attribute__((packed));

struct gdt_tss_descriptor {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t limit_high : 4;
    uint8_t flags      : 4;
    uint8_t base_high;
    uint32_t base_upper;
    uint32_t reserved;
} __attribute__((packed));

struct tss_entry {
    uint32_t reserved;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved2;
    uint64_t ist[7];
    uint64_t reserved3;
    uint16_t reserved4;
    uint16_t iomap_base;
} __attribute__((packed));

struct gdt_pointer {
    uint16_t limit;
    struct gdt_entry *base;
} __attribute__((packed));

struct gdt_entry gdt_entries[7];
struct tss_entry tss;
struct gdt_pointer gdtr;

void set_gdt_descriptor(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt_entries[index].limitLow = limit & 0xFFFF;
    gdt_entries[index].baseLow = base & 0xFFFF;
    gdt_entries[index].baseMiddle = (base >> 16) & 0xFF;
    gdt_entries[index].access = access;
    gdt_entries[index].limitHigh = (limit >> 16) & 0xF;
    gdt_entries[index].flags = flags;
    gdt_entries[index].baseHigh = (base >> 24) & 0xFF;
}

extern void load_gdt(struct gdt_pointer *gdtr_pointer);
extern void load_tss(uint16_t selector);

void init_tss() {
    tss = (struct tss_entry){0};
    tss.iomap_base = sizeof(struct tss_entry);

    struct gdt_tss_descriptor *descriptor = (struct gdt_tss_descriptor*) &gdt_entries[5];

    uint32_t limit = sizeof(struct tss_entry) - 1;
    uint64_t base = (uint64_t) &tss;

    descriptor->limit_low = limit & 0xFFFF;
    descriptor->base_low = base & 0xFFFF;
    descriptor->base_middle = (base >> 16) & 0xFF;
    descriptor->access = GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_SYSTEM_SEGMENT | TSS_ACCESS_AVAILABLE;
    descriptor->limit_high = (limit >> 16) & 0xF;
    descriptor->flags = 0;
    descriptor->base_high = (base >> 24) & 0xFF;
    descriptor->base_upper = (base >> 32) & 0xFFFFFFFF;
    descriptor->reserved = 0;
}

void init_gdt() {
    gdtr.limit = sizeof(gdt_entries) - 1;
    gdtr.base = gdt_entries;

    // null descriptor (0x00)
    set_gdt_descriptor(0, 0x00, 0x00, 0, 0);
    // kernel code segment (0x08)
    set_gdt_descriptor(1, 0x00, 0xFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE, GDT_FLAG_LONG_MODE | GDT_FLAG_GRANULARITY_4K);
    // kernel data segment (0x10)
    set_gdt_descriptor(2, 0x00, 0xFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITABLE, GDT_FLAG_PROTECTED_MODE | GDT_FLAG_GRANULARITY_4K);
    // user data segment (0x18)
    set_gdt_descriptor(3, 0x00, 0xFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITABLE, GDT_FLAG_PROTECTED_MODE | GDT_FLAG_GRANULARITY_4K);
    // user code segment (0x20)
    set_gdt_descriptor(4, 0x00, 0xFFFF, GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE, GDT_FLAG_LONG_MODE | GDT_FLAG_GRANULARITY_4K);
    // task state segment (0x28)
    init_tss();

    load_gdt(&gdtr);
    load_tss(0x28);
}
