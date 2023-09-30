#include "common.h"
#include "task.h"

extern void print_init(void);
extern void print_handle(TASK *task);
extern void exit_init(void);
extern void exit_handle(TASK *task);

static TASK s_task_list[] = {
    {
        .id = TASK_ID_PRINT,
        .times = -1,
        .delay = 0,
        .init = print_init,
        .handle = print_handle,
    },
    {
        .id = TASK_ID_EXIT,
        .times = -1,
        .delay = 0,
        .init = exit_init,
        .handle = exit_handle,
    },
};
const static uint32_t s_task_list_size = ARRAY_SIZE(s_task_list);

void *task_list_get(uint32_t *num) {
    *num = s_task_list_size;
    return s_task_list;
}
