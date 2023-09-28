#ifndef __TASK_USER_H__
#define __TASK_USER_H__

#include <stdint.h>

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

void *task_list_get(uint32_t *num);

#ifdef __cplusplus
}
#endif

#endif
