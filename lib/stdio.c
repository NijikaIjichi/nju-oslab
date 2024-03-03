#include "lib.h"

#define R_BUF_SIZE 256
#define W_BUF_SIZE 256
#define MAX_FILE   16

typedef struct FILE {
  int r_head, r_tail;
  char r_buf[R_BUF_SIZE];
  int w_tail;
  char w_buf[W_BUF_SIZE];
} FILE;

FILE file_table[MAX_FILE];

typedef struct istream {
  char (*getchar)(struct istream *self);
  char (*lookchar)(struct istream *self);
} istream_t;

typedef struct ifstream {
  istream_t in;
  int fd;
  FILE *fp;
} ifstream_t;

static int ifstream_refill(ifstream_t *self) {
  self->fp->r_head = 0;
  self->fp->r_tail = read(self->fd, self->fp->r_buf, R_BUF_SIZE);
  return self->fp->r_tail;
}

static char ifstream_getchar(ifstream_t *self) {
  if (self->fp->r_head == self->fp->r_tail) {
    if (ifstream_refill(self) == 0) return 0;
  }
  return self->fp->r_buf[self->fp->r_head++];
}

static char ifstream_lookchar(ifstream_t *self) {
  if (self->fp->r_head == self->fp->r_tail) {
    if (ifstream_refill(self) == 0) return 0;
  }
  return self->fp->r_buf[self->fp->r_head];
}

static int ifstream_init(ifstream_t *self, int fd) {
  if (fd < 0 || fd >= MAX_FILE) return -1;
  self->in.getchar = (void*)ifstream_getchar;
  self->in.lookchar = (void*)ifstream_lookchar;
  self->fd = fd;
  self->fp = &file_table[fd];
  return 0;
}

static void stream_remove_space(istream_t *in) {
  char ch;
  while ((ch = in->lookchar(in)) && isspace(ch)) {
    in->getchar(in);
  }
}

static void stream_gets(istream_t *in, char *str, size_t size, const char *delim, int throw, int contain) {
  for (size_t i = 0; i < size - 1; ++i) {
    char ch = in->lookchar(in);
    if (ch == 0) goto fin;
    if (strchr(delim, ch)) {
      if (contain) *str++ = in->getchar(in);
      goto fin;
    }
    *str++ = in->getchar(in);
  }
  if (throw) {
    while (1) {
      char ch = in->lookchar(in);
      if (ch == 0 || strchr(delim, ch)) goto fin;
      in->getchar(in);
    }
  }
fin:
  *str = 0;
}

static int stream_scanf(istream_t *in, const char *fmt, void **args) {
  const char *p;
  char buf[32];
  int res = 0;
  for (p = fmt; *p; ++p) {
    if (isspace(*p)) {
      stream_remove_space(in);
    } else if (*p == '%') {
      switch (*++p) {
      case 'c': *(char*)(*args++) = in->getchar(in); ++res; break;
      case 'd': stream_remove_space(in); stream_gets(in, buf, 32, SPACE, 1, 0); 
                *(int*)(*args++) = strtol(buf, NULL, 10); ++res; break;
      case 'u': stream_remove_space(in); stream_gets(in, buf, 32, SPACE, 1, 0); 
                *(uint32_t*)(*args++) = strtoul(buf, NULL, 10); ++res; break;
      case 'x': stream_remove_space(in); stream_gets(in, buf, 32, SPACE, 1, 0); 
                *(uint32_t*)(*args++) = strtoul(buf, NULL, 16); ++res; break;
      case 's': stream_remove_space(in); stream_gets(in, (char*)(*args++), -1, SPACE, 0, 0); ++res; break;
      case '%': if (in->lookchar(in) == '%') in->getchar(in); break;
      }
    } else if (*p == in->lookchar(in)) {
      in->getchar(in);
    }
  }
  return res;
}

int scanf(const char *fmt, ...) {
  ifstream_t ifs;
  if (ifstream_init(&ifs, STD_IN) < 0) return -1;
  return stream_scanf(&ifs.in, fmt, (void**)(&fmt) + 1);
}

char getchar() {
  ifstream_t ifs;
  if (ifstream_init(&ifs, STD_IN) < 0) return 0;
  return ifs.in.getchar(&ifs.in);
}

void gets(char *str, int size) {
  ifstream_t ifs;
  if (ifstream_init(&ifs, STD_IN) < 0) return;
  stream_gets(&ifs.in, str, size, "\n", 0, 1);
}

typedef struct ostream {
  void (*putchar)(struct ostream *self, char ch);
  void (*flush)(struct ostream *self);
} ostream_t;

typedef struct ofstream {
  ostream_t out;
  int fd;
  FILE *fp;
} ofstream_t;

static void ofstream_flush(ofstream_t *self) {
  write(self->fd, self->fp->w_buf, self->fp->w_tail);
  self->fp->w_tail = 0;
}

static void ofstream_putchar(ofstream_t *self, char ch) {
  self->fp->w_buf[self->fp->w_tail++] = ch;
  if (self->fp->w_tail == W_BUF_SIZE) {
    ofstream_flush(self);
  }
}

static int ofstream_init(ofstream_t *self, int fd) {
  if (fd < 0 || fd >= MAX_FILE) return -1;
  self->out.putchar = (void*)ofstream_putchar;
  self->out.flush = (void*)ofstream_flush;
  self->fd = fd;
  self->fp = &file_table[fd];
  return 0;
}

static void stream_puts(ostream_t *out, const char *str) {
  const char *p;
  for (p = str; *p; ++p) out->putchar(out, *p);
}

static void stream_printf(ostream_t *out, const char *fmt, uint32_t *args) {
  const char *p;
  char buf[32];
  for (p = fmt; *p; ++p) {
    if (*p != '%') out->putchar(out, *p);
    else {
      switch (*++p) {
      case 'c': out->putchar(out, *(char*)(args++)); break;
      case 'd': itoa(*args++, buf, 10); stream_puts(out, buf); break;
      case 'u': utoa(*args++, buf, 10); stream_puts(out, buf); break;
      case 'x': utoa(*args++, buf, 16); stream_puts(out, buf); break;
      case 's': stream_puts(out, *(char**)(args++)); break;
      default: out->putchar(out, *p);
      }
    }
  }
  out->flush(out);
}

void printf(const char *fmt, ...) {
  ofstream_t ofs;
  if (ofstream_init(&ofs, STD_OUT) < 0) return;
  stream_printf(&ofs.out, fmt, (uint32_t*)(&fmt) + 1);
}

void fprintf(int fd, const char *fmt, ...) {
  ofstream_t ofs;
  if (ofstream_init(&ofs, fd) < 0) return;
  stream_printf(&ofs.out, fmt, (uint32_t*)(&fmt) + 1);
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}
