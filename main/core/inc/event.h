#ifndef __EVENT_H__
#define __EVENT_H__

#include <stdint.h>

#include "ring_fifo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _EVENT {
    uint32_t type;
    void *custom_data;
} EVENT;

int8_t event_put(RING_FIFO *ring, EVENT *ev);
int8_t event_get(RING_FIFO *ring, EVENT *ev);
int8_t event_peek(RING_FIFO *ring, EVENT *ev);
uint16_t event_count(RING_FIFO *ring);
int8_t event_empty(RING_FIFO *ring);
int8_t event_full(RING_FIFO *ring);

#ifdef __cplusplus
}
#endif

#endif
