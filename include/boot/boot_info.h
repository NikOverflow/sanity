#pragma once

#include <stdint.h>

typedef struct {
    uint32_t *address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
} Framebuffer;

typedef struct {
    Framebuffer *framebuffer;
} BootInfo;
