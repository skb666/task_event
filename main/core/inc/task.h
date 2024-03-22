#ifndef __TASK_H__
#define __TASK_H__

#include <stdint.h>

#include "event.h"
#include "task_user.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _TASK_EVENT {
    EVENT_TYPE type;
    volatile uint32_t number_of_subscribe;
    uint32_t subscribers[EVENT_SUBSCRIBE_MAX];
} TASK_EVENT;

typedef struct _TASK {
    uint32_t id;
    volatile int32_t times;
    volatile uint32_t delay;
    void (*init)(void);
    void (*handle)(struct _TASK *);
    EVENT events_buffer[TASK_EVENT_MAX];
    RING_FIFO events;
} TASK;

void task_update_times(TASK *task);
void task_delay_ms(uint32_t id, uint32_t ms);

int8_t task_event_subscribe(EVENT_TYPE type, uint32_t id);
int8_t task_event_unsubscribe(EVENT_TYPE type, uint32_t id);
int8_t task_event_publish(EVENT_TYPE type, void *data, uint32_t priority);

void task_init(void);
void task_loop(void);
void task_time_loop(void);

#ifdef __cplusplus
}
#endif

#endif
