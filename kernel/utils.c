#include <stdint.h>

void delay(volatile uint32_t count)
{
    for (volatile uint32_t i = 0; i < count; i++);
}