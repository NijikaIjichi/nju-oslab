#include "ulib.h"

#define BUF_SIZE 256

static char buf[BUF_SIZE];
static int head, tail;

static int refill() {
  head = 0;
  tail = read(0, buf, BUF_SIZE);
  return tail;
}

char getchar() {
  if (head == tail && refill() == 0) {
    return 0;
  }
  return buf[head++];
}

static char peekchar() {
  if (head == tail && refill() == 0) {
    return 0;
  }
  return buf[head];
}

static void remove_space() {
  char ch;
  while ((ch = peekchar()) && isspace(ch)) {
    getchar();
  }
}

static void gets(char *buf, size_t size, const char *delim, 
                 bool throw, bool contain) {
  for (size_t i = 0; i < size - 1; ++i) {
    char ch = peekchar();
    if (ch == 0) goto fin;
    if (strchr(delim, ch)) {
      if (contain) *buf++ = getchar();
      goto fin;
    }
    *buf++ = getchar();
  }
  if (throw) {
    while (1) {
      char ch = peekchar();
      if (ch == 0 || strchr(delim, ch)) goto fin;
      getchar();
    }
  }
fin:
  *buf = 0;
}

char *getline(char *buf, size_t size) {
  gets(buf, size, "\n", false, true);
  return buf;
}

typedef long (*atol_t)(const char *__restrict__, char **__restrict__, int);

int scanf(const char *format, ...) {
  const char *p;
  char buf[32];
  int res = 0;
  va_list args;
  va_start(args, format);
  for (p = format; *p; ++p) {
    if (isspace(*p)) {
      remove_space();
    } else if (*p == '%') {
      ++res;
      int base = 0;
      bool sign = false;
      switch (*++p) {
      case 'c': *va_arg(args, char*) = getchar(); continue;
      case 's': remove_space();
                gets(va_arg(args, char*), -1, " \t\n", true, false); continue;
      case 'd': sign = true;
      case 'u': base = 10; break;
      case 'x': base = 16; break;
      default: p--; res--; continue;
      }
      remove_space();
      gets(buf, sizeof(buf), " \t\n", true, false);
      *va_arg(args, long*) = (sign ? strtol : (atol_t)strtoul)(buf, NULL, base);
    } else if (*p == peekchar()) {
      getchar();
    }
  }
  va_end(args);
  return res;
}
