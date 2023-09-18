#include "common.h"
#include "task.h"
#include "timer.h"

extern void print_init(void);
extern void print_handle(TASK *task);
extern void toggle_b_init(void);
extern void toggle_b_handle(TASK *task);
extern void exit_init(void);
extern void exit_handle(TASK *task);

static TASK s_task_list[] = {
    {
        .id = TASK_ID_PRINT,
        .times = -1,
        .init = print_init,
        .handle = print_handle,
    },
    {
        .id = TASK_ID_EXIT,
        .times = -1,
        .init = exit_init,
        .handle = exit_handle,
    },
};
const static uint32_t s_task_list_size = ARRAY_SIZE(s_task_list);

TASK *task_list_get(uint32_t *num) {
    *num = s_task_list_size;
    return s_task_list;
}

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

        if (ev.type == EVENT_TYPE_TICK_1S) {
            printf("from tick_1s: %u\n", timer_get_tick());
        } else if (ev.type == EVENT_TYPE_TICK_5S) {
            printf("from tick_5s: %u\n", timer_get_tick());
            task_event_publish(EVENT_TYPE_EXIT, NULL);
        }
    }

    task_update_times(task);
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
