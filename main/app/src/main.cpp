#include "common.h"
#include "task.h"
#include "timer.h"
#include "timer.hpp"
#include "timing_wheel.h"
#include "timer.h"

void task_public_event(void *event) {
    task_event_publish((EVENT_TYPE)(size_t)event, NULL, 0);
}

TIMER_TASK timer_tasks[] = {
    {
        .type = TIMER_TASK_TYPE_PERIODIC,
        .times = -1,
        .period = {
            .hour = 0,
            .min = 0,
            .sec = 1,
            .msec = 0,
        },
        .callback = task_public_event,
        .param = (void *)EVENT_TYPE_TICK_1S,
    },
    {
        .type = TIMER_TASK_TYPE_PERIODIC,
        .times = 1,
        .period = {
            .hour = 0,
            .min = 0,
            .sec = 5,
            .msec = 0,
        },
        .callback = task_public_event,
        .param = (void *)EVENT_TYPE_TICK_5S,
    },
};

void main_init(void) {
    task_init();

    timing_wheel_disable();
    timing_wheel_init();
    timing_wheel_set(0, 0, 0, 0);
    timing_wheel_add_tasks(timer_tasks, ARRAY_SIZE(timer_tasks));
    timing_wheel_enable();
}

void main_loop(void) {
    timing_wheel_run();
    
    task_loop();
}

int main() {
    Timer systick;
    systick.start(1, [] {
        timing_wheel_tick();
        timer_increase();
        // timer_loop();
        task_time_loop();
    });

    main_init();
    while (1) {
        main_loop();
    }

    return 0;
}
