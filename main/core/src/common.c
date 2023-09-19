#include "common.h"

#include <stdint.h>

#ifndef __x86_64__
#include "main.h"
#endif

static uint32_t s_irq_count = 0;

void reset_global_irq(void) {
    s_irq_count = 0;
}

void disable_global_irq(void) {
#ifndef __x86_64__
    __disable_irq();
#endif
    s_irq_count++;
    return;
}

void enable_global_irq(void) {
    if (s_irq_count != 0) {
        s_irq_count--;
    }

    if (0 == s_irq_count) {
#ifndef __x86_64__
        __enable_irq();
#endif
    }
    return;
}
