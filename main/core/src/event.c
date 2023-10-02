#include "event.h"

#include "common.h"

static int event_compare(const void *ev_a, const void *ev_b) {
    if (((EVENT *)ev_a)->priority > ((EVENT *)ev_b)->priority) {
        return 1;
    } else if (((EVENT *)ev_a)->priority < ((EVENT *)ev_b)->priority) {
        return -1;
    } else {
        return 0;
    }
}

int8_t event_push(RING_FIFO *ring, EVENT *ev) {
    int8_t err = 0;

    disable_global_irq();
    err = ring_push(ring, (void *)ev);
    enable_global_irq();

    return err;
}

int8_t event_binsert(RING_FIFO *ring, EVENT *ev) {
    int8_t err = 0;

    disable_global_irq();
    err = ring_binsert(ring, (void *)ev, event_compare);
    enable_global_irq();

    return err;
}

int8_t event_pop(RING_FIFO *ring, EVENT *ev) {
    int8_t err = 0;

    disable_global_irq();
    err = ring_pop(ring, ev);
    enable_global_irq();

    return err;
}

int8_t event_pop_only(RING_FIFO *ring) {
    int8_t err = 0;

    disable_global_irq();
    err = ring_pop_unread(ring);
    enable_global_irq();

    return err;
}

int8_t event_peek(RING_FIFO *ring, EVENT **ev) {
    *ev = ring_peek(ring);
    if (*ev == NULL) {
	return -1;
    }

    return 0;
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
