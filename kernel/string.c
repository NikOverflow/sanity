#include <string.h>

#include <stdint.h>

void *memcpy(void *dest, void *src, size_t n) {
    uint8_t *dest_ptr = dest;
    uint8_t *src_ptr = src;
    for(size_t i = 0; i < n; i++) {
        dest_ptr[i] = src_ptr[i];
    }
    return dest;
}

void *memset(void *dest, int value, size_t n) {
    uint8_t *ptr = dest;
    for(size_t i = 0; i < n; i++) {
        ptr[i] = (uint8_t) value;
    }
    return dest;
}
