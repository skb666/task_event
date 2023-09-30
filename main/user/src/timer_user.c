#include "common.h"
#include "timer.h"

static TIMER s_timer_list[] = {
    {
        .event_type = EVENT_TYPE_TICK_25MS,
        .priority = 0,
        .reload = TICK_25MS,
        .tick = TICK_25MS,
        .times = 0,
    },
    {
        .event_type = EVENT_TYPE_TICK_1S,
        .priority = 2,
        .reload = TICK_1S,
        .tick = TICK_1S,
        .times = -1,
    },
    {
        .event_type = EVENT_TYPE_TICK_5S,
        .priority = 1,
        .reload = TICK_5S,
        .tick = TICK_5S,
        .times = 1,
    },
};
const static uint32_t s_timer_list_size = ARRAY_SIZE(s_timer_list);

void *timer_list_get(uint32_t *num) {
    *num = s_timer_list_size;
    return s_timer_list;
}
