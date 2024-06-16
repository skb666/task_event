#include "timing_wheel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

static TIMING_WHEEL s_timing_wheel = {0};
static TIMER_TASK *s_suspend_tasks = NULL;

static inline void time_regulate(struct __TIME *time) {
    if (time == NULL) {
        return;
    }

    if (time->msec >= 1000) {
        time->sec += time->msec / 1000;
        time->msec %= 1000;
    }

    if (time->sec >= 60) {
        time->min += time->sec / 60;
        time->sec %= 60;
    }

    if (time->min >= 60) {
        time->hour += time->min / 60;
        time->min %= 60;
    }
}

static inline void time_add(struct __TIME *res, struct __TIME *time_a, struct __TIME *time_b) {
    if ((res == NULL) || (time_a == NULL) || (time_b == NULL)) {
        return;
    }

    res->hour = time_a->hour + time_b->hour;
    res->min = time_a->min + time_b->min;
    res->sec = time_a->sec + time_b->sec;
    res->msec = time_a->msec + time_b->msec;
}

static inline int8_t timer_task_push(TIMER_TASK **head, TIMER_TASK *task) {
    if (task == NULL) {
        return -1;
    }

    task->next = *head;
    *head = task;

    return 0;
}

static inline TIMER_TASK *timer_task_pop(TIMER_TASK **head) {
    TIMER_TASK *p = NULL;

    if (*head == NULL) {
        return NULL;
    }

    p = *head;
    *head = p->next;
    p->next = NULL;

    return p;
}

void timing_wheel_init(void) {
    disable_global_irq();
    memset(&s_timing_wheel, 0, sizeof(TIMING_WHEEL));
    enable_global_irq();
}

void timing_wheel_enable(void) {
    disable_global_irq();
    s_timing_wheel.enable = 1;
    enable_global_irq();
}

void timing_wheel_disable(void) {
    disable_global_irq();
    s_timing_wheel.enable = 0;
    enable_global_irq();
}

void timing_wheel_set(uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec) {
    disable_global_irq();
    s_timing_wheel.time.hour = hour;
    s_timing_wheel.time.min = min;
    s_timing_wheel.time.sec = sec;
    s_timing_wheel.time.msec = msec;
    enable_global_irq();
}

int timing_wheel_add_tasks(TIMER_TASK *tasks, int num) {
    int cnt = 0;

    if ((tasks == NULL) || (num <= 0)) {
        return 0;
    }

    for (int i = 0; i < num; ++i) {
        if (tasks[i].times == 0) {
            continue;
        }

        disable_global_irq();

        if (tasks[i].type == TIMER_TASK_TYPE_PERIODIC) {
            time_add(&tasks[i].time, &s_timing_wheel.time, &tasks[i].period);
        }
        time_regulate(&tasks[i].time);
        if (tasks[i].time.hour >= 24) {
            tasks[i].time.hour -= 24;
            timer_task_push(&s_suspend_tasks, &tasks[i]);
        } else {
            timer_task_push(&s_timing_wheel.hour_wheel[tasks[i].time.hour], &tasks[i]);
        }

        enable_global_irq();

        cnt++;
    }

    return cnt;
}

void timing_wheel_tick(void) {
    TIMER_TASK *p = NULL;
    TIMER_TASK *q = NULL;

    if (!s_timing_wheel.enable) {
        return;
    }

    disable_global_irq();

    s_timing_wheel.time.msec++;
    time_regulate(&s_timing_wheel.time);

    if (s_timing_wheel.time.hour >= 24) {
        s_timing_wheel.time.hour = 0;
        p = s_suspend_tasks;
        s_suspend_tasks = NULL;
        while (p) {
            q = timer_task_pop(&p);
            if (q->time.hour >= 24) {
                q->time.hour -= 24;
                timer_task_push(&s_suspend_tasks, q);
            } else {
                timer_task_push(&s_timing_wheel.hour_wheel[q->time.hour], q);
            }
        }
    }

    enable_global_irq();
}

void timing_wheel_run(void) {
    TIMER_TASK *p = NULL;
    TIMER_TASK *q = NULL;

    if (!s_timing_wheel.enable) {
        return;
    }

    disable_global_irq();

    p = s_timing_wheel.hour_wheel[s_timing_wheel.time.hour];
    s_timing_wheel.hour_wheel[s_timing_wheel.time.hour] = NULL;
    while (p) {
        q = timer_task_pop(&p);
        if (q->time.min != s_timing_wheel.time.min) {
            timer_task_push(&s_timing_wheel.min_wheel[q->time.min], q);
        } else if (q->time.sec != s_timing_wheel.time.sec) {
            timer_task_push(&s_timing_wheel.sec_wheel[q->time.sec], q);
        } else {
            timer_task_push(&s_timing_wheel.msec_wheel[q->time.msec], q);
        }
    }

    p = s_timing_wheel.min_wheel[s_timing_wheel.time.min];
    s_timing_wheel.min_wheel[s_timing_wheel.time.min] = NULL;
    while (p) {
        q = timer_task_pop(&p);
        if (q->time.sec != s_timing_wheel.time.sec) {
            timer_task_push(&s_timing_wheel.sec_wheel[q->time.sec], q);
        } else {
            timer_task_push(&s_timing_wheel.msec_wheel[q->time.msec], q);
        }
    }

    p = s_timing_wheel.sec_wheel[s_timing_wheel.time.sec];
    s_timing_wheel.sec_wheel[s_timing_wheel.time.sec] = NULL;
    while (p) {
        q = timer_task_pop(&p);
        timer_task_push(&s_timing_wheel.msec_wheel[q->time.msec], q);
    }

    p = s_timing_wheel.msec_wheel[s_timing_wheel.time.msec];
    s_timing_wheel.msec_wheel[s_timing_wheel.time.msec] = NULL;
    while (p) {
        q = timer_task_pop(&p);

        if (q->callback) {
            q->callback(q->param);
        }

        if (q->times > 0) {
            q->times--;
        }

        if (q->times) {
            if (q->type == TIMER_TASK_TYPE_SCHEDULED) {
                q->time.hour += 24;
            }
            timing_wheel_add_tasks(q, 1);
        }
    }

    enable_global_irq();
}
