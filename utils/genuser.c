#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libgen.h>
#include <assert.h>

#define MAX_NAME (31 - 2 * sizeof(uint32_t))
#define SECTSIZE 512
#define MAX_FILE (SECTSIZE / sizeof(inode_t))

typedef struct {
  uint32_t start_sect;
  uint32_t length;
  char name[MAX_NAME + 1];
} inode_t;

inode_t inode[MAX_FILE];

int file_num = 0, curr_sect = 257;
FILE *disk;
char buf[SECTSIZE];

void add_file(char *name) {
  FILE *fp = fopen(name, "r");
  assert(fp);
  assert(file_num < MAX_FILE);
  char *bsname = basename(name);
  assert(strlen(bsname) <= MAX_NAME);
  inode[file_num].start_sect = curr_sect;
  strcpy(inode[file_num].name, bsname);
  fseek(fp, 0, SEEK_END);
  uint32_t length = inode[file_num].length = ftell(fp);
  curr_sect += (length + SECTSIZE - 1) / SECTSIZE;
  fseek(fp, 0, SEEK_SET);
  ++file_num;
  size_t rdbytes;
  while ((rdbytes = fread(buf, 1, SECTSIZE, fp)) > 0) {
    if (rdbytes < SECTSIZE) {
      memset(&buf[rdbytes], 0, SECTSIZE - rdbytes);
    }
    fwrite(buf, 1, SECTSIZE, disk);
  }
  fclose(fp);
}

void write_inode() {
  fseek(disk, 0, SEEK_SET);
  fwrite(inode, 1, SECTSIZE, disk);
}

int main(int argc, char *argv[]) {
  assert(argc > 2);
  disk = fopen(argv[1], "w");
  assert(disk);
  fwrite(buf, SECTSIZE, 1, disk);
  for (int i = 2; i < argc; ++i) {
    add_file(argv[i]);
  }
  write_inode();
  fclose(disk);
  return 0;
}
