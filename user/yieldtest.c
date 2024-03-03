#include "lib.h"

const char *text[] = {"This is app2.\n",
                      "If you see this message, your task switch works well!\n",
											"Congratulations!\n"};

int main(int argc, char *argv[]) {
	for (int i = 0; i < sizeof(text) / sizeof(text[0]); ++i) {
		for (volatile int j = 0; j < 200000000; ++j) ;
		printf(text[i]);
		yield();
	}
	for (int i = 0; i < argc; ++i) {
		for (volatile int j = 0; j < 200000000; ++j) ;
		printf("argv[%d] = %s\n", i, argv[i]);
		yield();
	}
	return 0;
}
