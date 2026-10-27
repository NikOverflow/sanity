#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include <boot/boot_info.h>
#include <kernel/kprint.h>
#include <kernel/font.h>
#include <string.h>

static uint64_t cursor_x = 0;
static uint64_t cursor_y = 0;

void draw_pixel(uint64_t x, uint64_t y, uint32_t color) {
    if(x < 0 || x >= framebuffer->width || y < 0 || y >= framebuffer->height) return;
    size_t index = y * (framebuffer->pitch / 4) + x;
    framebuffer->address[index] = color;
}

void draw_char(char c, uint64_t x, uint64_t y, uint32_t foreground_color) {
    unsigned int char_pitch  = gimp_image.width;
    unsigned int char_width  = 8;
    unsigned int char_height = 16;

    const unsigned int index = char_width * (unsigned int)((uint8_t)c);

    for(unsigned int row = 0; row < char_height; row++) {
        for(unsigned int column = 0; column < char_width; column++) {
            unsigned int base = (index + row * char_pitch + column) * gimp_image.bytes_per_pixel;
            draw_pixel(x + column, y + row, (*((uint32_t*)(gimp_image.pixel_data + base)) & foreground_color));
        }
    }
}

void kputc(char c) {
    if(c == '\n') {
        cursor_x = 0;
        cursor_y += 16;
    } else if(c == ' ') {
        cursor_x += 8;
    } else {
        draw_char(c, cursor_x, cursor_y, 0xFFFFFF);
        cursor_x += 8;
    }
    if(cursor_x + 8 > framebuffer->width) {
        cursor_x = 0;
        cursor_y += 16;
    }
    if(cursor_y + 16 > framebuffer->height) {
        cursor_x = 0;
        cursor_y = 0;
    }
}

void kprint(char *string) {
    for(size_t i = 0; string[i] != '\0'; i++) {
        kputc(string[i]);
    }
}

char *kitoa(int value, int base) {
    char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

    static char buffer[34];
    int32_t i = 33;
    buffer[i] = '\0';

    int is_negative = 0;
    uint32_t uvalue = (uint32_t) value;
    if(value < 0 && base == 10) {
        is_negative = 1;
        uvalue = (uint32_t) (-value);
    }

    if(uvalue == 0) {
        buffer[--i] = '0';
    } else {
        while(uvalue > 0) {
            buffer[--i] = digits[uvalue % base];
            uvalue /= base;
        }
    }

    if(is_negative) buffer[--i] = '-';

    return &buffer[i];
}


void kprintf(char *format, ...) {
    va_list args;
    va_start(args, format);
    for(size_t i = 0; format[i] != '\0'; i++) {
        if(format[i] != '%') {
            kputc(format[i]);
            continue;
        }
        i++;
        switch(format[i]) {
            case '%':
                kputc('%');
                break;
            case 'd':
                kprint(kitoa(va_arg(args, int), 10));
                break;
            default:
                break;
        }
    }
    va_end(args);
}
