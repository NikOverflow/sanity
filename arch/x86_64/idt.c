#include <stdint.h>

#include <idt.h>

#define IDT_TYPE_INTERRUPT 0x0E
#define IDT_TYPE_TRAP      0x0F

#define IDT_FLAG_RING0   0b000
#define IDT_FLAG_RING1   0b001
#define IDT_FLAG_RING2   0b010
#define IDT_FLAG_RING3   0b011
#define IDT_FLAG_PRESENT 0b100

typedef struct {
    uint16_t baseLow;
    uint16_t segmentSelector;
    uint8_t ist: 3;
    uint8_t reserved: 5;
    uint8_t type: 4;
    uint8_t reserved2: 1;
    uint8_t flags: 3;
    uint16_t baseMiddle;
    uint32_t baseHigh;
    uint32_t reserved3;
} __attribute__((packed)) idtEntry;

typedef struct {
    uint16_t limit;
    idtEntry *base;
} __attribute__((packed)) idtPointer;

extern void load_idt(idtPointer *idtrPointer);

idtEntry idtEntries[256];
idtPointer idtr;

extern void *isr_stub_table[256];

void set_idt_gate(uint8_t interrupt, void *base, uint16_t segmentSelector, uint8_t ist, uint8_t type, uint8_t flags) {
    uintptr_t address = (uintptr_t) base;
    idtEntries[interrupt].baseLow = address & 0xFFFF;
    idtEntries[interrupt].segmentSelector = segmentSelector;
    idtEntries[interrupt].ist = ist;
    idtEntries[interrupt].reserved = 0x00;
    idtEntries[interrupt].type = type;
    idtEntries[interrupt].reserved2 = 0x00;
    idtEntries[interrupt].flags = flags;
    idtEntries[interrupt].baseMiddle = (address >> 16) & 0xFFFF;
    idtEntries[interrupt].baseHigh = (address >> 32) & 0xFFFFFFFF;
    idtEntries[interrupt].reserved3 = 0x00;
}

void init_idt() {
    idtr.limit = sizeof(idtEntries) - 1;
    idtr.base = idtEntries;

    for(uint16_t i = 0; i < 256; i++) {
        set_idt_gate(i, isr_stub_table[i], 0x08, 0, IDT_TYPE_INTERRUPT, IDT_FLAG_PRESENT | IDT_FLAG_RING0);
    }
    load_idt(&idtr);
}
