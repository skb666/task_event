#ifndef __EVENT_H__
#define __EVENT_H__

#include <stdint.h>

#include "ring_fifo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _EVENT {
    uint32_t type;
    uint32_t priority;
    void *custom_data;
} EVENT;

int8_t event_push(RING_FIFO *ring, EVENT *ev);
int8_t event_binsert(RING_FIFO *ring, EVENT *ev);
int8_t event_pop(RING_FIFO *ring, EVENT *ev);
int8_t event_pop_only(RING_FIFO *ring);
int8_t event_peek(RING_FIFO *ring, EVENT **ev);
uint16_t event_count(RING_FIFO *ring);
int8_t event_empty(RING_FIFO *ring);
int8_t event_full(RING_FIFO *ring);

#ifdef __cplusplus
}
#endif

#endif
