#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

void init_timer();
void timer_handle();
uint32_t get_tick();

#endif
