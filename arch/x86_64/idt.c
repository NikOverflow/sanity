#include <stdint.h>

typedef struct {
} __attribute__((packed)) idtEntry;

typedef struct {
    uint16_t limit;
    idtEntry *base;
} __attribute__((packed)) gdtPointer;
