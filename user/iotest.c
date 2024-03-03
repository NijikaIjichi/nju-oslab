#include "lib.h"

char str[128];

int main() {
	printf("I/O test begin...\n");
	printf("the answer should be:\n");
	printf("#######################################################\n");
	printf("Hello, welcome to OSlab! I'm the body of the game.\n");
	printf("Now I will test your printf:\n");
	printf("1 + 1 = 2, 123 * 456 = 56088, 0, -1, -2147483648, -1412505855, -32768, 102030, 0, ffffffff, 80000000, abcdef01, ffff8000, 18e8e\n");
	printf("Now I will test your getChar: ");
	printf("1 + 1 = 2\n2 * 123 = 246\n");
	printf("Now I will test your getStr: ");
	printf("Alice is stronger than Bob\nBob is weaker than Alice\n");
	printf("#######################################################\n");
	printf("your answer:\n");
	printf("=======================================================\n");
	printf("%s %s%scome %co%s", "Hello,", "", "wel", 't', " ");
	printf("%c%c%c%c%c! ", 'O', 'S', 'l', 'a', 'b');
	printf("I'm the %s of %s.\n", "body", "the game");
	printf("Now I will test your printf:\n");
	printf("%d + %d = %d, %d * %d = %d, ", 1, 1, 1 + 1, 123, 456, 123 * 456);
	printf("%d, %d, %d, %d, %d, %d, ", 0, 0xffffffff, 0x80000000, 0xabcedf01, -32768, 102030);
	printf("%x, %x, %x, %x, %x, %x\n", 0, 0xffffffff, 0x80000000, 0xabcedf01, -32768, 102030);
	printf("Now I will test your getchar: ");
	printf("1 + 1 = ");
	char num = getchar(); getchar();
	printf("%c * 123 = 246\n",num);
	printf("Now I will test your gets: ");
	printf("Alice is stronger than ");
	char name[20];
	gets(name, 20);
	printf("%s is stronger than Alice\n",name);
	printf("=======================================================\n");
	printf("Extra Test for scanf\n");
	printf("Input 2 decimal numbers: ");
	int a, b, times = 0; scanf("%d%d", &a, &b);
	printf("%d + %d = %d\n", a, b, a + b);
	if (a == 114514 || b == 114514 || a == 1919810 || b == 1919810) {
		printf("How can you input such a kusai number!\n");
		++times;
	}
	printf("Input 2 hex numbers: ");
	uint32_t c, d; scanf("%x%x", &c, &d);
	printf("0x%x + 0x%x = 0x%x\n", c, d, c + d);
	if (c == 0x114514 || d == 0x114514 || c == 0x1919810 || d == 0x1919810) {
		printf("How can you input such a kusai number%s!\n", times ? " again" : "");
	}
	printf("Fill me: _imbo, ________ \n");
	char ch;
	scanf(" %c%s", &ch, str);
	printf("%cimbo, %s\n", ch, str);
	if ((ch == 'J' || ch == 'j') && (strcmp(str, "NMSMSHSA") == 0 || strcmp(str, "nmsmshsa") == 0)) {
		printf("Aren't you thanksgiving to NJU?\n");
	}
	printf("Test end!!! Good luck!!!\n");
	return 0;
}
