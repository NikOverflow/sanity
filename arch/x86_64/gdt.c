#include <stdint.h>

#include <gdt.h>

typedef struct {
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMiddle;
    uint8_t segmentType: 4;
    uint8_t descriptorType: 1;
    uint8_t privilegeLevel: 2;
    uint8_t present: 1;
    uint8_t limitHigh: 4;
    uint8_t available: 1;
    uint8_t longMode: 1;
    uint8_t operationSize: 1;
    uint8_t granularity: 1;
    uint8_t baseHigh;
} __attribute__((packed)) gdtEntry;

typedef struct {
    uint16_t limit;
    gdtEntry *base;
} __attribute__((packed)) gdtPointer;

gdtEntry gdtEntries[5];
gdtPointer gdtr;

void set_gate(uint8_t number, uint32_t limit, uint32_t base, uint8_t segmentType, uint8_t descriptorType, uint8_t privilegeLevel, uint8_t longMode, uint8_t operationSize, uint8_t granularity) {
    gdtEntries[number].limitLow = limit & 0xFFFF;
    gdtEntries[number].baseLow = base & 0xFFFF;
    gdtEntries[number].baseMiddle = (base >> 16) & 0xFF;
    gdtEntries[number].segmentType = segmentType;
    gdtEntries[number].descriptorType = descriptorType;
    gdtEntries[number].privilegeLevel = privilegeLevel;
    gdtEntries[number].present = 1;
    gdtEntries[number].limitHigh = (limit >> 16) & 0xF;
    gdtEntries[number].available = 0;
    gdtEntries[number].longMode = longMode;
    gdtEntries[number].operationSize = operationSize;
    gdtEntries[number].granularity = granularity;
    gdtEntries[number].baseHigh = (base >> 24) & 0xFF;
}

extern void load_gdt(gdtPointer *gdtrPointer);

void init_gdt() {
    gdtr.limit = (sizeof(gdtEntry) * 5) - 1;
    gdtr.base = gdtEntries;

    set_gate(0, 0, 0, 0, 0, 0, 0, 0, 0);
    set_gate(1, 0, 0, 10, 1, 0, 1, 0, 0); // Kernel Code (0x08)
    set_gate(2, 0, 0, 2, 1, 0, 0, 0, 0); // Kernel Data (0x10)
    set_gate(3, 0, 0, 10, 1, 3, 1, 0, 0); // User Code (0x18)
    set_gate(4, 0, 0, 2, 1, 3, 0, 0, 0); // User Data (0x20)

    load_gdt(&gdtr);
}
