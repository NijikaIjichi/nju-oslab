#include "ulib.h"
#define MAXARG 31

#define SBUFSIZE 4096
static char static_buf[SBUFSIZE];
static char *new_argv[MAXARG + 1];

void parse_arg(char *args, int argc, char *argv[]) {
    if (!args || !*args) {return;}
    strtok(args, " \t");
    do {
        argv[argc++] = args;
        if (argc > MAXARG) {fprintf(2, "xargs: too many argv\n"); exit(1);}
    } while ((args = strtok(NULL, " \t")));
    argv[argc] = NULL;
}

void exec_another(char *file, char *argv[]) {
    int r;
    switch (r = fork()) {
    case -1: fprintf(2, "xargs: fork failed\n"); exit(1);
    case 0: exec(file, argv); fprintf(2, "xargs: exec failed\n"); exit(1);
    default: wait(NULL);
    }
}

void exec_by_args(char *args, char *file, int argc, char *argv[]) {
    char *end = args + strlen(args);
    char *this_line = strtok(args, "\n");
    if (!this_line) {return;}
    char *next_line = this_line + strlen(this_line) + 1;
    if (next_line > end) {next_line = NULL;}
    while (this_line) {
        parse_arg(this_line, argc, argv);
        exec_another(file, argv);
        this_line = strtok(next_line, "\n");
        if (!this_line) {return;}
        next_line = this_line + strlen(this_line) + 1;
        if (next_line > end) {next_line = NULL;}
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "xargs: missing operand\n");
        exit(1);
    }
    memmove(new_argv, &argv[1], (argc - 1) * sizeof(char*));
    if (read(0, static_buf, SBUFSIZE) < SBUFSIZE) {
        exec_by_args(static_buf, argv[1], argc - 1, new_argv);
    } else {
        int buf_size = SBUFSIZE * 2;
        char* buf = malloc(buf_size);
        if (!buf) {fprintf(2, "xargs: malloc failed\n"); exit(1);}
        memmove(buf, static_buf, SBUFSIZE);
        while (read(0, buf + buf_size / 2, buf_size / 2) == buf_size / 2) {
            char* new_buf = malloc(buf_size *= 2);
            memmove(new_buf, buf, buf_size / 2);
            free(buf);
            buf = new_buf;
        }
        exec_by_args(buf, argv[1], argc - 1, new_argv);
        free(buf);
    }
    exit(0);
}
