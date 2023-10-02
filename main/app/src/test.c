#include <stdio.h>

#include "task.h"
#include "timer.h"

void task_event_process(TASK *task, void (*callback)(EVENT *)) {
    int8_t err;
    EVENT *ev;

    if (event_empty(&task->events)) {
        return;
    }

    while (event_count(&task->events)) {
        err = event_peek(&task->events, &ev);
        if (err) {
            return;
        }

        callback(ev);

        event_pop_only(&task->events);
        task_update_times(task);
    }
}

void print_init(void) {
    task_event_subscribe(EVENT_TYPE_TICK_1S, TASK_ID_PRINT);
    task_event_subscribe(EVENT_TYPE_TICK_5S, TASK_ID_PRINT);
}

void print_cb(EVENT *ev) {
    switch (ev->type) {
        case EVENT_TYPE_TICK_1S: {
            printf("tick_1s: %u\n", timer_get_tick());
        } break;
        case EVENT_TYPE_TICK_5S: {
            printf("tick_5s: %u\n", timer_get_tick());
            task_event_publish(EVENT_TYPE_EXIT, NULL, 0);
            task_delay_ms(TASK_ID_EXIT, 5000);
        } break;
        default: {
        } break;
    }
}

void print_handle(TASK *task) {
    task_event_process(task, print_cb);
}

void exit_init(void) {
    task_event_subscribe(EVENT_TYPE_EXIT, TASK_ID_EXIT);
}

void exit_cb(EVENT *ev) {
    if (ev->type == EVENT_TYPE_EXIT) {
        printf("exit\n");
        exit(0);
    }
}

void exit_handle(TASK *task) {
    task_event_process(task, exit_cb);
}
