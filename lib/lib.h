#ifndef __lib_h__
#define __lib_h__
#include "types.h"

struct stat {
  uint32_t node;
  uint32_t type;
  uint32_t size;
};

void printf(const char *format, ...);
void fprintf(int fd, const char *format, ...);
char getchar();
void gets(char *str, int size);
int scanf(const char *format, ...);
int stat(const char *n, struct stat *st);

char *itoa(int value, char *str, int base);
char *utoa(unsigned value, char *str, int base);
long strtol(const char *str, char **endptr, int base);
unsigned long strtoul(const char *str, char **endptr, int base);
int rand();
void srand(unsigned int seed);
void *sbrk(int increment);
void *malloc(uint32_t size);
void free(void *ptr);
#define atoi(str) (strtol((str), NULL, 10))

int strcmp(const char *s1, const char *s2);
char *strchr(const char *s1, int i);
char *strtok(char *str, const char *delim);
uint32_t strlen(const char *s);
void memcpy(void *dst, const void *src, uint32_t size);
void memset(void *dst, int c, uint32_t size);
void strcpy(char *dst, const char *src);

#define MAX_NAME (31 - sizeof(uint32_t))
typedef struct dirent {uint32_t node; char name[MAX_NAME + 1];} dirent_t;

int write(int fd, const void *buf, uint32_t size);
int read(int fd, void *buf, uint32_t size);
void yield();
int fork();
void sleep(int time);
int wait(int *status);
void exit(int status) __attribute__((noreturn));
int exec(const char *path, char *argv[]);
int open(const char *path, int mode);
int dup(int fd);
int pipe(int fd[2]);
void close(int fd);
uint32_t lseek(int fd, uint32_t off, int whence);
int kill(int pid);
int mkdir(const char *path);
int chdir(const char *path);
int unlink(const char *path);
int fstat(int fd, struct stat *st);
int brk(void *addr);
int getpid();

typedef int sem_t[2];
int sem_init(sem_t sem, int val);
void sem_wait(sem_t sem);
void sem_post(sem_t sem);
void sem_destroy(sem_t sem);
#define P(sem) (sem_wait(sem))
#define V(sem) (sem_post(sem))

typedef struct {
  void *ptr;
  uint32_t size;
  int fd[2];
} svar_t;
int svar_init(svar_t *svar, void *ptr, uint32_t size);
void svar_read(svar_t *svar);
void svar_write(svar_t *svar);
void svar_close(svar_t *svar);

#define RAND_MAX 2147483647
#define LONG_MIN -2147483648L
#define LONG_MAX 2147483647L
#define ULONG_MAX (LONG_MAX * 2UL + 1UL)
#define isspace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n')
#define SPACE " \t\n"

#define STD_IN  0
#define STD_OUT 1
#define STD_ERR 2

#define TYPE_NONE 0
#define TYPE_FILE 1
#define TYPE_DIR  2
#define TYPE_DEV  3

#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200
#define O_TRUNC   0x400

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#endif
