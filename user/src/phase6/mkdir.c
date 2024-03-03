#include "ulib.h"

int
main(int argc, char *argv[])
{
  int i, fd;

  if(argc < 2){
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }

  for(i = 1; i < argc; i++){
    fd = open(argv[i], O_CREATE | O_DIR);
    if(fd < 0){
      fprintf(2, "mkdir: %s failed to create\n", argv[i]);
      break;
    }
    close(fd);
  }

  exit(0);
}