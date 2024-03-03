#include "lib.h"
#include "types.h"



int uEntry(void) {
	int data = 0;	
	
	int ret = fork();
	int i = 8;
	if (ret == 0) {
		data = 2;
		while( i != 0) {
			i --;
			printf("Child Process: Pong %d, %d;\n", data, i);
			sleep(60);
		}
		exec(221, 20);
		exit();
	}
	else if (ret != -1) {
		data = 1;
		while( i != 0) {
			i --;
			printf("Father Process: Ping %d, %d;\n", data, i);
			sleep(60);
		}
		exit();
	}

	return 0;
}
