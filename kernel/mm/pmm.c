#include <stddef.h>

#include <kernel/mm/pmm.h>
#include <printf.h>

#define PAGE_SIZE 4096
#define MAX_ORDER 10

#define PAGE_RESERVED   (1 << 0)
#define PAGE_FREE       (1 << 1)

#define PAGE_ALIGN_UP(value) (((value) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_DOWN(value) ((value) & ~(PAGE_SIZE - 1))

struct Page {
    uint8_t order;
    uint8_t flags: 2;
    struct Page *next;
    struct Page *prev;
};

static uint64_t total_pages = 0;
static struct Page *pages = NULL;
static struct Page *free_lists[MAX_ORDER + 1] = {NULL};

void list_add(struct Page *page, uint8_t order) {
    page->order = order;
    page->flags |= PAGE_FREE;
    page->next = free_lists[order];
    page->prev = NULL;
    if(free_lists[order] != NULL) {
        free_lists[order]->prev = page;
    }
    free_lists[order] = page;
}

void list_remove(struct Page *page, uint8_t order) {
    if(page->prev != NULL) {
        page->prev->next = page->next;
    } else {
        free_lists[order] = page->next;
    }
    if(page->next != NULL) {
        page->next->prev = page->prev;
    }
    page->next = NULL;
    page->prev = NULL;
    page->flags &= ~PAGE_FREE;
}

void init_pmm(struct memory_map *memory_map) {
    uint64_t highest_addr = 0;
    for(size_t i = 0; i < memory_map->regionCount; i++) {
        struct memory_region *region = memory_map->regions[i];
        if(region->type != MEMORY_MAP_USABLE) continue;
        if(memory_map->offset == 0 && region->base == 0) { // Thanks limine bootloader for that.
            region->base += 1;
            region->length -= 1;
        }
        uint64_t top = region->base + region->length;
        if(top > highest_addr) highest_addr = top;
    }
    total_pages = highest_addr / PAGE_SIZE;
    printf("Total Pages: %llu\n", total_pages);

    size_t array_size = PAGE_ALIGN_UP(total_pages * sizeof(struct Page));
    printf("Page array size: %llu bytes\n", array_size);

    uint64_t metadata_phys_addr = 0; // TODO: There are many stupid firmware devs so I should make this better
    for(size_t i = 0; i < memory_map->regionCount; i++) {
        struct memory_region *region = memory_map->regions[i];
        if(region->type != MEMORY_MAP_USABLE || region->length < array_size) continue;
        metadata_phys_addr = region->base;
        region->base += array_size;
        region->length -= array_size;
        break;
    }
    pages = (struct Page*)(metadata_phys_addr + memory_map->offset);

    for(uint64_t i = 0; i < total_pages; i++) {
        pages[i].order = 0;
        pages[i].flags = PAGE_RESERVED;
        pages[i].next = NULL;
        pages[i].prev = NULL;
    }

    uint64_t total_free_bytes = 0;
    printf("Usable Regions:\n");
    for(size_t i = 0; i < memory_map->regionCount; i++) {
        struct memory_region *region = memory_map->regions[i];
        if(region->type != MEMORY_MAP_USABLE) continue;
        uint64_t base = PAGE_ALIGN_UP(region->base);
        uint64_t end = PAGE_ALIGN_DOWN(region->base + region->length);
        printf("0x%llx - 0x%llx\n", base, end);

        while(base < end) {
            uint64_t chunk_size = end - base;

            uint8_t order = MAX_ORDER;
            while(order > 0) {
                uint64_t size = (1ULL << order) * PAGE_SIZE;
                if(chunk_size >= size && (base % size) == 0) break;
                order--;
            }

            uint64_t pfn = base / PAGE_SIZE;
            pages[pfn].flags &= ~PAGE_RESERVED;
            list_add(&pages[pfn], order);

            uint64_t block_size = (1ULL << order) * PAGE_SIZE;
            total_free_bytes += block_size;
            base += block_size;
        }
    }
    printf("PMM Initialized. Available Memory: %llu MiB (%llu Pages free)\n", (total_free_bytes / (1024 * 1024)), (total_free_bytes / PAGE_SIZE));
}

uint64_t alloc(uint8_t order) {
    if(order > MAX_ORDER) return 0;

    uint8_t current_order = order;
    while(current_order <= MAX_ORDER && free_lists[current_order] == NULL) current_order++;
    if(current_order > MAX_ORDER) return 0;

    struct Page *page = free_lists[current_order];
    uint64_t phys_addr = (page - pages) * PAGE_SIZE;
    list_remove(page, current_order);

    while(current_order > order) {
        current_order--;
        uint64_t buddy_phys = phys_addr + ((1ULL << current_order) * PAGE_SIZE);
        uint64_t buddy_pfn = buddy_phys / PAGE_SIZE;
        pages[buddy_pfn].flags &= ~PAGE_RESERVED;
        list_add(&pages[buddy_pfn], current_order);
    }
    page->order = order;
    return phys_addr;
}

uint64_t alloc_page() {
    return alloc(0);
}

void free_page(uint64_t phys_addr) {
    uint64_t pfn = phys_addr / PAGE_SIZE;
    if(pfn >= total_pages) return;

    struct Page *page = &pages[pfn];

    uint8_t current_order = page->order;
    while(current_order < MAX_ORDER) {
        uint64_t buddy_pfn = pfn ^ (1ULL << current_order);
        if(buddy_pfn >= total_pages) break;
        struct Page *buddy = &pages[buddy_pfn];
        if(!(buddy->flags & PAGE_FREE) || (buddy->flags & PAGE_RESERVED) || buddy->order != current_order) break;
        list_remove(buddy, current_order);
        if(buddy_pfn < pfn) {
            pfn = buddy_pfn;
            page = buddy;
        }
        current_order++;
    }
    list_add(page, current_order);
}
