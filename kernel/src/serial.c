#include "klib.h"
#include "serial.h"
#include "proc.h"
#include "sem.h"

#define SERIAL_PORT 0x3F8

static sem_t serial_sem;

void init_serial() {
  outb(SERIAL_PORT + 1, 0x00); // you don't need to understand this
  outb(SERIAL_PORT + 3, 0x80); // I don't understand either :)
  outb(SERIAL_PORT + 0, 0x01);
  outb(SERIAL_PORT + 1, 0x00);
  outb(SERIAL_PORT + 3, 0x03);
  outb(SERIAL_PORT + 2, 0xC7);
  outb(SERIAL_PORT + 4, 0x0B);
  outb(SERIAL_PORT + 1, 0x01);
  putchar('\n'); // start a new line
  sem_init(&serial_sem, 0);
}

static inline bool serial_idle() {
  return (inb(SERIAL_PORT + 5) & 0x20) != 0;
}

void putchar(char ch) {
  while (!serial_idle()) ;
  outb(SERIAL_PORT, ch);
}

#define BUFFER_SIZE 256

static char buffer[BUFFER_SIZE];
static int head, tail, clapboard;

static void push_back(char ch) {
  buffer[tail++ % BUFFER_SIZE] = ch;
  // TODO: Lab2-4 V(sem) tail-clapboard times if ch=='\n'
  if (ch == '\n') {
    while (clapboard != tail) {
      clapboard++;
      sem_v(&serial_sem);
    }
  }
}

static char pop_back() {
  if (tail > clapboard) {
    return buffer[--tail % BUFFER_SIZE];
  }
  return 0;
}

static char pop_front() {
  if (clapboard > head) {
    return buffer[head++ % BUFFER_SIZE];
  }
  return 0;
}

void serial_handle() {
  char ch;
  while (inb(SERIAL_PORT + 5) & 0x01) {
    ch = inb(SERIAL_PORT);
    switch (ch) {
    case 0: continue; // no more input

    case '\x1b': // special key (e.g. arrow key), ignore it
      while (inb(SERIAL_PORT + 5) & 0x01) inb(SERIAL_PORT);
      continue;

    case '\b':
    case '\x7f': // backspace, remove a char from buffer tail is possible
      if (pop_back()) {
        putchar('\b'); putchar(' '); putchar('\b'); // remove it from screen
      }
      continue;

    case '\r': ch = '\n'; // Enter
    }
    // printable char
    if (ch == '\n' || (ch >= ' ' && ch < 127)) {
      putchar(ch); // put the char to screen
      push_back(ch); // put it to the buffer, if it's '\n', move clapboard to tail
    }
  }
}

char getchar() {
  char ch;
  //while ((ch = pop_front()) == 0) {
  //  //sti(); hlt(); cli();
  //  proc_yield();
  //}
  // TODO: Lab2-4 rewrite getchar with sem, P(sem) then pop_front
  sem_p(&serial_sem);
  ch = pop_front();
  assert(ch);
  return ch;
}

int serial_write(const void *buf, size_t count) {
  putstrn(buf, count);
  return count;
}

int serial_read(void *buf, size_t count) {
  char ch = 0;
  int i = 0;
  for (; i < count && ch != '\n'; ++i) {
    ch = ((char*)buf)[i] = getchar();
  }
  return i;
}
