#include "timer.h"

#include "task.h"

volatile static uint32_t s_timer_tick = 0;

void timer_increase(void) {
    s_timer_tick++;
}

uint32_t timer_get_tick(void) {
    return s_timer_tick;
}

void timer_loop(void) {
    uint32_t list_size;
    TIMER *timer_list;

    timer_list = timer_list_get(&list_size);

    for (uint32_t index = 0; index < list_size; ++index) {
        if (timer_list[index].tick > 0) {
            timer_list[index].tick -= 1;
        }
        if (timer_list[index].times && timer_list[index].tick == 0) {
            timer_list[index].tick = timer_list[index].reload;
            if (timer_list[index].times > 0) {
                timer_list[index].times -= 1;
            }
            if (task_event_publish(timer_list[index].event_type, NULL, timer_list[index].priority)) {
                // printf("[ERROR] event_publish\n");
                return;
            }
        }
    }
}
