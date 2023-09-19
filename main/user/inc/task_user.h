#ifndef __TASK_USER_H__
#define __TASK_USER_H__

#include <stdint.h>

#include "event.h"

#define EVENT_FIFO_MAX 256
#define EVENT_SUBSCRIBE_MAX 5
#define TASK_EVENT_MAX 10

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _EVENT_TYPE {
    EVENT_TYPE_TICK_25MS,
    EVENT_TYPE_TICK_1S,
    EVENT_TYPE_TICK_5S,
    EVENT_TYPE_EXIT,

    EVENT_TYPE_MAX,
} EVENT_TYPE;

typedef enum _TASK_ID {
    TASK_ID_PRINT,
    TASK_ID_EXIT,

    TASK_ID_MAX,
} TASK_ID;

typedef struct _TASK_EVENT {
    EVENT_TYPE type;
    uint32_t number_of_subscribe;
    uint32_t subscribers[EVENT_SUBSCRIBE_MAX];
} TASK_EVENT;

typedef struct _TASK {
    uint32_t id;
    int32_t times;
    void (*init)(void);
    void (*handle)(struct _TASK *);
    EVENT events_buffer[TASK_EVENT_MAX];
    RING_FIFO events;
} TASK;

TASK *task_list_get(uint32_t *num);

#ifdef __cplusplus
}
#endif

#endif
