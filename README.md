# 事件驱动多任务框架


## 编译运行

**安装** [PyBuildKit](https://github.com/skb666/PyBuildKit)

```bash
curl -fsSL https://raw.githubusercontent.com/skb666/PyBuildKit/master/install.sh | bash
. $HOME/.mysdk/env
```

**编译**

```bash
python ./project build
```

**运行**

```bash
python ./project run
```

## 接口说明

### 主程序接口

周期运行以下接口，作为内部定时器及定时信号的驱动源

```c
timer_increase();
timer_loop();
```

主程序中用以下接口对所有任务做初始化

```c
task_init();
```

程序主循环中调用以下接口以实时处理用户任务

```c
task_loop();
```

### 用户定时器

用户定时器用于周期性产生定时器事件

**添加用户定时器**

1. 在 [task_user.h](main/user/inc/task_user.h) 的 `EVENT_TYPE` 中添加定时器到达指定时间后产生的事件名称
2. 在 [timer_user.h](main/user/inc/timer_user.h) 的 `TIMER_TICK` 中添加对应事件触发需要的时间
3. 在 [timer_user.c](test_sample/main/user/src/timer_user.c) 的 `s_timer_list` 中以 `{事件名称, 重载间隔, 起始滴答值},` 的格式来增加一个定时器

### 用户任务

用户任务结构体如下：

```c
typedef struct _TASK {
    uint32_t id;                            // 任务 ID
    int32_t times;                          // 任务剩余执行次数，负数表示无穷次
    void (*init)(void);                     // 任务初始化回调
    void (*handle)(struct _TASK *);         // 任务处理回调
    EVENT events_buffer[TASK_EVENT_MAX];    // 内部使用，作为下面字段的 buffer
    RING_FIFO events;                       // 任务事件缓冲区
} TASK;
```

当主函数中执行 `task_init()` 时会遍历用户任务列表，对每个任务做初始化并执行任务初始化回调。

在任务初始化回调中一般进行事件注册，使用接口如下：

```c
int8_t task_event_subscribe(EVENT_TYPE type, uint32_t id);      // 订阅任务事件
int8_t task_event_unsubscribe(EVENT_TYPE type, uint32_t id);    // 取消订阅任务事件
int8_t task_event_publish(EVENT_TYPE type, void *data);         // 发布任务事件
```

每当有任务事件产生时，内部会先将任务事件分发给订阅了该事件的任务，保存于任务事件缓冲区以进行下一步处理。

任务处理回调中可以从 `TASK` 的 `events` 接收事件，并进行对应处理；除此之外，可以通过发布任务事件来进行任务事件编排，在 [task_user.h](main/user/inc/task_user.h) 的 `EVENT_TYPE` 中可以添加自定义任务事件。

当结束事件处理后，可以通过以下接口来减少任务剩余执行次数：

```c
void task_update_times(TASK *task);
```

> 问：为什么不将“任务剩余次数递减”作为内部流程？
> 答：任务回调中进行了事件处理，会有“指定事件处理完成后才算一次处理”的需求，开放接口可以使更加灵活。

**添加用户任务**

1. 在 [task_user.h](main/user/inc/task_user.h) 的 `TASK_ID` 中添加任务 ID
2. 在 [task_user.c](test_sample/main/user/src/task_user.c) 的 `s_task_list` 中添加任务，格式参照结构体 `TASK`
