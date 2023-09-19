#include "event.h"

#include "common.h"

int8_t event_put(RING_FIFO *ring, EVENT *ev) {
    int8_t err = 0;

    disable_global_irq();
    err = ring_push(ring, (void *)ev);
    enable_global_irq();

    return err;
}

int8_t event_get(RING_FIFO *ring, EVENT *ev) {
    int8_t err = 0;

    disable_global_irq();
    err = ring_pop(ring, ev);
    enable_global_irq();

    return err;
}

int8_t event_peek(RING_FIFO *ring, EVENT *ev) {
    int8_t err = 0;

    err = ring_peek(ring, ev);

    return err;
}

uint16_t event_count(RING_FIFO *ring) {
    return ring_size(ring);
}

int8_t event_empty(RING_FIFO *ring) {
    return ring_is_empty(ring);
}

int8_t event_full(RING_FIFO *ring) {
    return ring_is_full(ring);
}
