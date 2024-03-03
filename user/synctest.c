#include "lib.h"

int main() {
  int dec = 0;
  int hex = 0;
  char str[6];
  char cha = 0;
  int ret = 0;
  while(1){
    printf("Input:\" Test %%c Test %%s %%d %%x\"\n");
    ret = scanf(" Test %c Test %s %d %x", &cha, str, &dec, &hex);
    printf("Ret: %d; %c, %s, %d, %x.\n", ret, cha, str, dec, hex);
    if (ret == 4)
      break;
  }
  
  // 测试信号量
  int i = 4;
  sem_t sem;
  printf("Father Process: Semaphore Initializing.\n");
  ret = sem_init(sem, 0);
  if (ret == -1) {
    printf("Father Process: Semaphore Initializing Failed.\n");
    exit(0);
  }

  ret = fork();
  if (ret == 0) {
    while( i != 0) {
      i --;
      printf("Child Process: Semaphore Waiting.\n");
      sem_wait(sem);
      printf("Child Process: In Critical Area.\n");
    }
    printf("Child Process: Semaphore Destroying.\n");
    sem_destroy(sem);
    exit(0);
  }
  else if (ret != -1) {
    while( i != 0) {
      i --;
      printf("Father Process: Sleeping.\n");
      sleep(128);
      printf("Father Process: Semaphore Posting.\n");
      sem_post(sem);
    }
    printf("Father Process: Semaphore Destroying.\n");
    sem_destroy(sem);
    exit(0);
  }
  return 0;
}
