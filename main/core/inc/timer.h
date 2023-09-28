#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

#include "task_user.h"
#include "timer_user.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _TIMER {
    EVENT_TYPE event_type;
    uint32_t reload;
    uint32_t tick;
    int32_t times;
} TIMER;

void timer_increase(void);
uint32_t timer_get_tick(void);
void timer_loop(void);

#ifdef __cplusplus
}
#endif

#endif
