#include "timer_conf.h"

#include "common.h"

static TIMER s_timer_list[] = {
    {EVENT_TYPE_TICK_25MS, TICK_25MS, TICK_25MS},
    {EVENT_TYPE_TICK_1S, TICK_1S, TICK_1S},
    {EVENT_TYPE_TICK_5S, TICK_5S, TICK_5S},
};
const static uint32_t s_timer_list_size = ARRAY_SIZE(s_timer_list);

TIMER *timer_list_get(uint32_t *num) {
    *num = s_timer_list_size;
    return s_timer_list;
}
