#include <stdio.h>

#include "task.h"
#include "timer.h"

void print_init(void) {
    task_event_subscribe(EVENT_TYPE_TICK_1S, TASK_ID_PRINT);
    task_event_subscribe(EVENT_TYPE_TICK_5S, TASK_ID_PRINT);
}

void print_handle(TASK *task) {
    int8_t err;
    EVENT ev;

    if (event_empty(&task->events)) {
        return;
    }

    while (event_count(&task->events)) {
        err = event_get(&task->events, &ev);
        if (err) {
            return;
        }

        switch (ev.type) {
            case EVENT_TYPE_TICK_1S: {
                printf("tick_1s: %u\n", timer_get_tick());
            } break;
            case EVENT_TYPE_TICK_5S: {
                printf("tick_5s: %u\n", timer_get_tick());
                task_event_publish(EVENT_TYPE_EXIT, NULL);
            } break;
            default: {
            } break;
        }

        task_update_times(task);
    }
}

void exit_init(void) {
    task_event_subscribe(EVENT_TYPE_EXIT, TASK_ID_EXIT);
}

void exit_handle(TASK *task) {
    int8_t err;
    EVENT ev;

    if (event_empty(&task->events)) {
        return;
    }

    err = event_get(&task->events, &ev);
    if (err) {
        return;
    }

    if (ev.type == EVENT_TYPE_EXIT) {
        task_update_times(task);

        printf("exit\n");

        exit(0);
    }
}
