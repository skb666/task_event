#ifndef __TIMING_WHEEL_H__
#define __TIMING_WHEEL_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TIMER_TASK_TYPE_PERIODIC,
    TIMER_TASK_TYPE_SCHEDULED,
} TIMER_TASK_TYPE;

struct __TIME {
    volatile uint16_t hour;  // 时
    volatile uint16_t min;   // 分
    volatile uint16_t sec;   // 秒
    volatile uint16_t msec;  // 毫秒
};

typedef struct __TIMER_TASK {
    TIMER_TASK_TYPE type;      // 任务类型
    int times;                 // 执行次数
    struct __TIME time;        // 时间计数
    struct __TIME period;      // 周期
    void (*callback)(void *);  // 回调函数
    void *param;               // 回调参数
    struct __TIMER_TASK *next;
} TIMER_TASK;

typedef struct {
    int enable;                    // 计时器是否开启
    struct __TIME time;            // 时间计数
    TIMER_TASK *hour_wheel[24];    // 小时轮
    TIMER_TASK *min_wheel[60];     // 分钟轮
    TIMER_TASK *sec_wheel[60];     // 秒轮
    TIMER_TASK *msec_wheel[1000];  // 毫秒轮
} TIMING_WHEEL;

void timing_wheel_init(void);
void timing_wheel_enable(void);
void timing_wheel_disable(void);
void timing_wheel_set(uint16_t hour, uint16_t min, uint16_t sec, uint16_t msec);
int timing_wheel_add_tasks(TIMER_TASK *tasks, int num);
void timing_wheel_tick(void);
void timing_wheel_run(void);

#ifdef __cplusplus
}
#endif

#endif
