#include "ring_fifo.h"

#include <stdio.h>
#include <string.h>

int8_t ring_push(RING_FIFO *ring, const void *element) {
    uint8_t *pbuf = NULL;

    if (ring == NULL || element == NULL) {
        return -1;
    }

    /* 已满且不支持覆盖 */
    if (ring->size >= ring->capacity && !ring->cover) {
        return -1;
    }

    pbuf = (uint8_t *)ring->buffer + ring->tail * ring->element_size;
    memcpy(pbuf, element, ring->element_size);
    ring->tail = (ring->tail + 1) % ring->capacity;

    /* 已满但支持覆盖 */
    if (ring->size >= ring->capacity && ring->cover) {
        ring->head = (ring->head + 1) % ring->capacity;
    } else {
        ring->size += 1;
    }

    return 0;
}

int8_t ring_binsert(RING_FIFO *ring, const void *element, int (*compare)(const void *, const void *)) {
    uint8_t *pbuf = NULL;
    uint8_t *pbuf_tmp = NULL;
    NUM_TYPE_SIGNED i, left, right, mid;

    if (ring == NULL || element == NULL || compare == NULL) {
        return -1;
    }

    /* 支持覆盖或已满 */
    if (ring->cover || ring->size >= ring->capacity) {
        return -1;
    }

    if (ring->size < 1) {
        pbuf = (uint8_t *)ring->buffer + ring->tail * ring->element_size;
        memcpy(pbuf, element, ring->element_size);
        ring->tail = (ring->tail + 1) % ring->capacity;
        ring->size += 1;

        return 0;
    }

    left = 0;
    right = ring->size - 1;

    while (left <= right) {
        mid = (left + right) / 2;
        if (compare(element, (uint8_t *)ring->buffer + ((ring->head + mid) % ring->capacity) * ring->element_size) < 0) {
            // 从小到大：key 小，往左找
            // 从大到小：key 大，往左找
            right = mid - 1;
        } else {
            // 从小到大：key 大于等于，往右找
            // 从大到小：key 小于等于，往右找
            left = mid + 1;
        }
    }

    for (i = ring->size; i > left; --i) {
        pbuf = (uint8_t *)ring->buffer + ((ring->head + i) % ring->capacity) * ring->element_size;
        pbuf_tmp = (uint8_t *)ring->buffer + ((ring->head + i - 1) % ring->capacity) * ring->element_size;
        memcpy(pbuf, pbuf_tmp, ring->element_size);
    }

    pbuf = (uint8_t *)ring->buffer + ((ring->head + left) % ring->capacity) * ring->element_size;
    memcpy(pbuf, element, ring->element_size);
    ring->tail = (ring->tail + 1) % ring->capacity;
    ring->size += 1;

    return 0;
}

int8_t ring_pop(RING_FIFO *ring, void *element) {
    uint8_t *pbuf = NULL;

    if (ring == NULL || element == NULL || ring->size == 0) {
        return -1;
    }

    pbuf = (uint8_t *)ring->buffer + ring->head * ring->element_size;
    memcpy(element, pbuf, ring->element_size);
    ring->head = (ring->head + 1) % ring->capacity;

    ring->size -= 1;

    if (ring_is_empty(ring)) {
        ring_reset(ring);
    }

    return 0;
}

int8_t ring_pop_unread(RING_FIFO *ring) {
    if (ring == NULL || ring->size == 0) {
        return -1;
    }

    ring->head = (ring->head + 1) % ring->capacity;
    ring->size -= 1;

    if (ring_is_empty(ring)) {
        ring_reset(ring);
    }

    return 0;
}

NUM_TYPE ring_pop_mult_unread(RING_FIFO *ring, NUM_TYPE num) {
    NUM_TYPE cnt = 0;

    if (ring == NULL || ring->size == 0 || num == 0) {
        return 0;
    }

    if (num >= ring->size) {
        cnt = ring->size;
        ring_reset(ring);
    } else {
        cnt = num;
        ring->head = (ring->head + cnt) % ring->capacity;
        ring->size -= cnt;

        if (ring_is_empty(ring)) {
            ring_reset(ring);
        }
    }

    return cnt;
}

void *ring_peek(RING_FIFO *ring) {
    if (ring == NULL || ring->size == 0) {
        return NULL;
    }

    return (uint8_t *)ring->buffer + ring->head * ring->element_size;
}

void *ring_peek_next(RING_FIFO *ring, void *ptr) {
    NUM_TYPE index = 0;

    if (ring == NULL || ring->size == 0) {
        return NULL;
    }

    if (ptr == NULL) {
        // 返回头部指针
        return (uint8_t *)ring->buffer + ring->head * ring->element_size;
    }

    index = ((uint8_t *)ptr - (uint8_t *)ring->buffer) / ring->element_size + 1;
    index %= ring->capacity;
    if (index != ring->tail) {
        return (uint8_t *)ring->buffer + index * ring->element_size;
    }

    return NULL;
}

NUM_TYPE ring_push_mult(RING_FIFO *ring, const void *elements, NUM_TYPE num) {
    uint8_t *inbuf = NULL;
    uint8_t *outbuf = NULL;
    NUM_TYPE cnt = 0, remain = 0;
    NUM_TYPE tmp = 0;

    if (ring == NULL || elements == NULL || num == 0) {
        return 0;
    }

    /* 已满且不支持覆盖 */
    if (ring->size >= ring->capacity && !ring->cover) {
        return 0;
    }

    if (ring->size + num <= ring->capacity) {
        /* 放得下则全放 */
        cnt = num;
    } else {
        /* 放到填满为止 */
        cnt = ring->capacity - ring->size;
    }

    /* 未满时放入，直到填满为止 */
    if (cnt) {
        inbuf = (uint8_t *)elements;
        outbuf = (uint8_t *)ring->buffer + ring->tail * ring->element_size;
        if (ring->tail + cnt <= ring->capacity) {
            /* 放的时候不会够到数组边界 */
            memcpy(outbuf, inbuf, cnt * ring->element_size);
        } else {
            /* 放的时候会够到数组边界，分两次 */
            tmp = ring->capacity - ring->tail;
            memcpy(outbuf, inbuf, tmp * ring->element_size);
            inbuf += tmp * ring->element_size;
            outbuf = (uint8_t *)ring->buffer;
            memcpy(outbuf, inbuf, (cnt - tmp) * ring->element_size);
        }
        ring->tail = (ring->tail + cnt) % ring->capacity;
        ring->size += cnt;
    }

    /* 支持覆盖 */
    if (ring->cover) {
        remain = num - cnt;
        if (remain == 0) {
            /* 没有东西要放了 */
            return cnt;
        } else if (remain < ring->capacity) {
            /* 在放满后还需要部分覆盖 */
            inbuf = (uint8_t *)elements + cnt * ring->element_size;
            outbuf = (uint8_t *)ring->buffer + ring->head * ring->element_size;
            if (ring->head + remain <= ring->capacity) {
                /* 覆盖时不会够到数组边界 */
                memcpy(outbuf, inbuf, remain * ring->element_size);
            } else {
                /* 覆盖时会够到数组边界，分两次 */
                tmp = ring->capacity - ring->head;
                memcpy(outbuf, inbuf, tmp * ring->element_size);
                inbuf += tmp * ring->element_size;
                outbuf = (uint8_t *)ring->buffer;
                memcpy(outbuf, inbuf, (remain - tmp) * ring->element_size);
            }
            ring->head = (ring->head + remain) % ring->capacity;
            ring->tail = ring->head;
        } else {
            /* 在放满后需要全覆盖 */
            inbuf = (uint8_t *)elements + (num - ring->capacity) * ring->element_size;
            outbuf = (uint8_t *)ring->buffer;
            memcpy(outbuf, inbuf, ring->capacity * ring->element_size);
            ring->head = 0;
            ring->tail = 0;
        }
        return num;
    }

    return cnt;
}

NUM_TYPE ring_pop_mult(RING_FIFO *ring, void *elements, NUM_TYPE num) {
    uint8_t *inbuf = NULL;
    uint8_t *outbuf = NULL;
    NUM_TYPE cnt = 0;
    NUM_TYPE tmp = 0;

    if (ring == NULL || elements == NULL || num == 0 || ring->size == 0) {
        return 0;
    }

    if (num <= ring->size) {
        /* 取的数量不超过有的数量 */
        cnt = num;
    } else {
        /* 取得数量过大时，有几个取几个 */
        cnt = ring->size;
    }

    inbuf = (uint8_t *)ring->buffer + ring->head * ring->element_size;
    outbuf = (uint8_t *)elements;
    if (ring->head + cnt <= ring->capacity) {
        /* 取的时候不会够到数组边界 */
        memcpy(outbuf, inbuf, cnt * ring->element_size);
    } else {
        /* 取的时候会够到数组边界，分两次 */
        tmp = ring->capacity - ring->head;
        memcpy(outbuf, inbuf, tmp * ring->element_size);
        inbuf = (uint8_t *)ring->buffer;
        outbuf += tmp * ring->element_size;
        memcpy(outbuf, inbuf, (cnt - tmp) * ring->element_size);
    }

    ring->head = (ring->head + cnt) % ring->capacity;
    ring->size -= cnt;

    if (ring_is_empty(ring)) {
        ring_reset(ring);
    }

    return cnt;
}

int8_t ring_deep_copy(RING_FIFO *ring_dst, RING_FIFO *ring_src) {
    if ((ring_dst == NULL || ring_dst->buffer == NULL) || (ring_src == NULL || ring_src->buffer == NULL)) {
        return -1;
    }

    if ((ring_dst->capacity != ring_src->capacity) || (ring_dst->element_size != ring_src->element_size)) {
        return -1;
    }

    memcpy(ring_dst->buffer, ring_src->buffer, ring_src->capacity * ring_src->element_size);
    ring_dst->cover = ring_src->cover;
    ring_dst->head = ring_src->head;
    ring_dst->tail = ring_src->tail;
    ring_dst->size = ring_src->size;

    return 0;
}

void ring_reset(RING_FIFO *ring) {
    ring->head = 0;
    ring->tail = 0;
    ring->size = 0;
}

int8_t ring_is_empty(RING_FIFO *ring) {
    return (ring->size == 0);
}

int8_t ring_is_full(RING_FIFO *ring) {
    return (ring->size >= ring->capacity);
}

NUM_TYPE ring_size(RING_FIFO *ring) {
    return ring->size;
}

void print_ring(RING_FIFO *ring) {
    printf("========== ring ==========\n");
    printf("cover: %lu\n", (uint64_t)ring->cover);
    printf("element_size: %lu\n", (uint64_t)ring->element_size);
    printf("capacity: %lu\n", (uint64_t)ring->capacity);
    printf("head: %lu\n", (uint64_t)ring->head);
    printf("tail: %lu\n", (uint64_t)ring->tail);
    printf("size: %lu\n", (uint64_t)ring->size);
    printf("--------------------------\n");
}
