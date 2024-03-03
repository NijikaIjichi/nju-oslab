#include "klib.h"
#include "timer.h"
#include "proc.h"

#define TIMER_PORT 0x40
#define FREQ_8253 1193182
#define HZ 100

static uint32_t tick;

void init_timer() {
  int counter = FREQ_8253 / HZ;
  outb(TIMER_PORT + 3, 0x34);
  outb(TIMER_PORT + 0, counter % 256);
  outb(TIMER_PORT + 0, counter / 256);
}

void timer_handle() {
  ++tick;
  proc_yield(); // TODO: uncomment me in Lab2-1
}

uint32_t get_tick() {
  return tick;
}
