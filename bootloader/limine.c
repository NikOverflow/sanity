#include <stdint.h>

#include <kernel/kmain.h>

#include "limine/limine.h"

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);
__attribute__((used, section(".limine_requests")))

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

void limine_main() {
    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
    Framebuffer framebuffer = {
        .address = fb->address,
        .width = fb->width,
        .height = fb->height,
        .pitch = fb->pitch
    };
    BootInfo bootInfo = {
        .framebuffer = &framebuffer
    };
    kmain(&bootInfo);
}
