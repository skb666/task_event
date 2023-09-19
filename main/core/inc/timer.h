#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

#include "timer_user.h"

#ifdef __cplusplus
extern "C" {
#endif

void timer_increase(void);
uint32_t timer_get_tick(void);
void timer_loop(void);

#ifdef __cplusplus
}
#endif

#endif
