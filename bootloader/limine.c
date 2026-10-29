#include <stdint.h>
#include <stddef.h>

#include <kernel/kmain.h>

#include <limine.h>

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);
__attribute__((used, section(".limine_requests")))

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 6
};
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 6
};
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 6
};

void limine_main() {
    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
    struct limine_memmap_response *memmap = memmap_request.response;
    struct limine_hhdm_response *hhdm = hhdm_request.response;

    for(size_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];
        if(entry->type == LIMINE_MEMMAP_USABLE) {
            entry->type = MEMORY_MAP_USABLE;
        } else {
            entry->type = MEMORY_MAP_RESERVED;
        }
    }

    struct memory_map memory_map = {
        .region_count = memmap->entry_count,
        .offset = hhdm->offset,
        .regions = (struct memory_region**) memmap->entries
    };
    struct framebuffer framebuffer = {
        .address = fb->address,
        .width = fb->width,
        .height = fb->height,
        .pitch = fb->pitch
    };
    struct boot_info boot_info = {
        .framebuffer = &framebuffer,
        .memory_map = &memory_map
    };
    kmain(&boot_info);
}
