#include <stdint.h>
#include <stddef.h>

#include <kernel/kprint.h>
#include <kernel/font.h>

static uint64_t cursor_x = 0;
static uint64_t cursor_y = 0;

void draw_pixel(uint64_t x, uint64_t y, uint32_t color, Framebuffer *framebuffer) {
    if(x < 0 || x >= framebuffer->width || y < 0 || y >= framebuffer->height) return;
    size_t index = y * (framebuffer->pitch / 4) + x;
    framebuffer->address[index] = color;
}

void draw_char(char c, int x, int y, uint32_t foregroundColor, Framebuffer *framebuffer) {
    unsigned int charPitch  = gimpImage.width;
    unsigned int charWidth  = 8;
    unsigned int charHeight = 16;

    const unsigned int index = charWidth * (unsigned int)((uint8_t)c);

    for(unsigned int row = 0; row < charHeight; row++) {
        for(unsigned int column = 0; column < charWidth; column++) {
            unsigned int base = (index + row * charPitch + column) * gimpImage.bytes_per_pixel;
            draw_pixel(x + column, y + row, (*((uint32_t*)(gimpImage.pixel_data + base)) & foregroundColor), framebuffer);
        }
    }
}

void kprint(char *string, Framebuffer *framebuffer) {
    for(size_t i = 0; string[i] != '\0'; i++) {
        if(string[i] == '\n') {
            cursor_x = 0;
            cursor_y += 16;
            continue;
        }
        if(string[i] == ' ') {
            cursor_x += 8;
            continue;
        }
        draw_char(string[i], cursor_x, cursor_y, 0xFFFFFF, framebuffer);
        cursor_x += 8;
        if(cursor_x + 8 > framebuffer->width) {
            cursor_x = 0;
            cursor_y += 16;
        }
        if(cursor_y + 16 > framebuffer->height) {
            cursor_x = 0;
            cursor_y = 0;
        }
    }
}
