#include "ulib.h"

char*
fmtname(char *path)
{
  static char buf[MAX_NAME+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= MAX_NAME)
    return p;
  memcpy(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', MAX_NAME-strlen(p));
  return buf;
}

void
ls(char *path)
{
  int fd;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case TYPE_DEV:
  case TYPE_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.size, st.node);
    break;

  case TYPE_DIR:
    panic("not support dir");
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}

