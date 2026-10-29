#pragma once

#include <stdint.h>

#include <boot/boot_info.h>

void init_pmm(struct memory_map *memory_map);
uint64_t alloc(uint8_t order);
uint64_t alloc_page();
void free_page(uint64_t phys_addr);
