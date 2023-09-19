#ifndef __TASK_H__
#define __TASK_H__

#include <stdint.h>

#include "task_user.h"

#ifdef __cplusplus
extern "C" {
#endif

void task_update_times(TASK *task);

int8_t task_event_subscribe(EVENT_TYPE type, uint32_t id);
int8_t task_event_unsubscribe(EVENT_TYPE type, uint32_t id);
int8_t task_event_publish(EVENT_TYPE type, void *data);

void task_init(void);
void task_loop(void);

#ifdef __cplusplus
}
#endif

#endif
