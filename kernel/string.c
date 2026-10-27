#include <string.h>

void *memcpy(void *destination, void *source, size_t length) {
    char* dest = (char*) destination;
    char* src = (char*) source;
    for(size_t i = 0; i < length; i++) {
        dest[i] = src[i];
    }
    return destination;
}
