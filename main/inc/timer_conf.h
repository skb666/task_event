#ifndef __TIMER_CONF_H__
#define __TIMER_CONF_H__

#include <stdint.h>

#include "task_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _TIMER_TICK {
    TICK_25MS = 25,
    TICK_1S = 1000,
    TICK_5S = 5000,
} TIMER_TICK;

typedef struct _TIMER {
    EVENT_TYPE event_type;
    uint32_t reload;
    uint32_t tick;
} TIMER;

TIMER *timer_list_get(uint32_t *num);

#ifdef __cplusplus
}
#endif

#endif
