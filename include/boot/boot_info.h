#pragma once

#include <stdint.h>
#include <stddef.h>

#define MEMORY_MAP_USABLE   0
#define MEMORY_MAP_RESERVED 1

struct framebuffer {
    uint32_t *address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
};

struct memory_region {
    uintptr_t base;
    uintptr_t length;
    uint64_t type;
};

struct memory_map {
    size_t regionCount;
    uintptr_t offset;
    struct memory_region **regions;
};

struct boot_info {
    struct framebuffer *framebuffer;
    struct memory_map *memory_map;
};

extern struct framebuffer *framebuffer;
