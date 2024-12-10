#include "task.h"

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "timer.h"

ring_define_static(EVENT, s_event_fifo, EVENT_FIFO_MAX, 0);
static TASK_EVENT s_task_event_list[EVENT_TYPE_MAX];
volatile static uint8_t s_task_init = 0;

static int task_sort_cmp(const void *task_a, const void *task_b) {
    if (((TASK *)task_a)->id > ((TASK *)task_b)->id) {
        return 1;
    } else if (((TASK *)task_a)->id < ((TASK *)task_b)->id) {
        return -1;
    } else {
        return 0;
    }
}

static int task_find_cmp(const void *id, const void *task) {
    if (*(uint32_t *)id > ((TASK *)task)->id) {
        return 1;
    } else if (*(uint32_t *)id < ((TASK *)task)->id) {
        return -1;
    } else {
        return 0;
    }
}

void task_update_times(TASK *task) {
    if (task->times > 0) {
        task->times -= 1;
    }
}

void task_delay_ms(uint32_t id, uint32_t ms) {
    TASK *task, *task_list;
    uint32_t list_size;

    task_list = task_list_get(&list_size);

    task = bsearch(&id, task_list, list_size, sizeof(TASK), task_find_cmp);
    if (task) {
        disable_global_irq();
        task->delay = ms;
        enable_global_irq();
    }
}

static void task_event_init(void) {
    uint32_t type;

    memset(&s_task_event_list, 0, sizeof(s_task_event_list));

    for (type = 0; type < (uint32_t)EVENT_TYPE_MAX; ++type) {
        s_task_event_list[type].type = (EVENT_TYPE)type;
    }
}

int8_t task_event_subscribe(EVENT_TYPE type, uint32_t id) {
    uint32_t i, index;

    if (type >= EVENT_TYPE_MAX) {
        return -1;
    }

    index = s_task_event_list[type].number_of_subscribe;
    if (index >= EVENT_SUBSCRIBE_MAX) {
        return -1;
    }

    for (i = 0; i < index; ++i) {
        if (s_task_event_list[type].subscribers[i] == id) {
            // 已经订阅
            return -1;
        }
    }

    disable_global_irq();
    s_task_event_list[type].subscribers[index] = id;
    s_task_event_list[type].number_of_subscribe += 1;
    enable_global_irq();

    return 0;
}

int8_t task_event_unsubscribe(EVENT_TYPE type, uint32_t id) {
    uint32_t i, j, index;

    if (type >= EVENT_TYPE_MAX) {
        return -1;
    }

    index = s_task_event_list[type].number_of_subscribe;

    for (i = 0; i < index; ++i) {
        if (s_task_event_list[type].subscribers[i] == id) {
            disable_global_irq();
            for (j = i; j < index - 1; ++j) {
                s_task_event_list[type].subscribers[j] = s_task_event_list[type].subscribers[j + 1];
            }
            s_task_event_list[type].number_of_subscribe -= 1;
            enable_global_irq();
            return 0;
        }
    }

    return -1;
}

int8_t task_event_publish(EVENT_TYPE type, void *data, uint32_t priority) {
    EVENT event;

    if (!s_task_init || event_full(&s_event_fifo)) {
        return -1;
    }

    event.type = type;
    event.custom_data = data;
    event.priority = priority;
    return event_binsert(&s_event_fifo, &event);
}

static void task_event_distribute(TASK *task_list, uint32_t list_size) {
    int8_t err = 0;
    EVENT *ev;
    uint32_t i;
    uint8_t flag;
    TASK *task = NULL;

    if (event_empty(&s_event_fifo)) {
        return;
    }

    if (task_list == NULL) {
        task_list = task_list_get(&list_size);
    }

    while (event_count(&s_event_fifo)) {
        err = event_peek(&s_event_fifo, &ev);
        if (err) {
            break;
        }
        flag = 0;
        for (i = 0; i < s_task_event_list[ev->type].number_of_subscribe; ++i) {
            task = bsearch(&s_task_event_list[ev->type].subscribers[i], task_list, list_size, sizeof(TASK), task_find_cmp);
            if (task && event_full(&task->events)) {
                flag = 1;
                break;
            }
        }
        if (flag) {
            break;
        }
        // 订阅者都有空余才分发
        for (i = 0; i < s_task_event_list[ev->type].number_of_subscribe; ++i) {
            task = bsearch(&s_task_event_list[ev->type].subscribers[i], task_list, list_size, sizeof(TASK), task_find_cmp);
            if (task && task->times) {
                event_push(&task->events, ev);
            }
        }
        event_pop_only(&s_event_fifo);
    }
}

void task_init(void) {
    uint32_t index, list_size;
    TASK *task_list;

    task_event_init();

    task_list = task_list_get(&list_size);

    for (index = 0; index < list_size; ++index) {
        task_list[index].events = (RING_FIFO){
            .buffer = task_list[index].events_buffer,
            .capacity = TASK_EVENT_MAX,
            .element_size = sizeof(EVENT),
            .cover = 0,
            .head = 0,
            .tail = 0,
            .size = 0,
        };
        if (task_list[index].init) {
            task_list[index].init();
        }
    }

    qsort(task_list, list_size, sizeof(TASK), task_sort_cmp);
    s_task_init = 1;
}

void task_loop(void) {
    uint32_t list_size;
    TASK *task_list;

    task_list = task_list_get(&list_size);

    task_event_distribute(task_list, list_size);

    for (uint32_t index = 0; index < list_size; ++index) {
        if (task_list[index].times && !task_list[index].delay) {
            if (task_list[index].handle) {
                task_list[index].handle(&task_list[index]);
            }
        }
    }
}

void task_time_loop(void) {
    uint32_t list_size;
    TASK *task_list;

    if (!s_task_init) {
        return;
    }

    timer_loop();

    task_list = task_list_get(&list_size);

    for (uint32_t index = 0; index < list_size; ++index) {
        if (task_list[index].delay > 0) {
            task_list[index].delay -= 1;
        }
    }
}
