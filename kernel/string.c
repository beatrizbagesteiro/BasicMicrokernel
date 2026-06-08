#include <stdint.h>

void *memset(void *s, int c, uint64_t n)
{
    uint8_t *p = s;
    while (n--)
        *p++ = c;
    return s;
}

void *memcpy(void *dest, const void *src, uint64_t n) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    for (uint64_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}