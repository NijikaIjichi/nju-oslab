#include "x86.h"
#include "device.h"

void initSerial(void) {
	outByte(SERIAL_PORT + 1, 0x00);
	outByte(SERIAL_PORT + 3, 0x80);
	outByte(SERIAL_PORT + 0, 0x01);
	outByte(SERIAL_PORT + 1, 0x00);
	outByte(SERIAL_PORT + 3, 0x03);
	outByte(SERIAL_PORT + 2, 0xC7);
	outByte(SERIAL_PORT + 4, 0x0B);
}

static inline int serialIdle(void) {
	return (inByte(SERIAL_PORT + 5) & 0x20) != 0;
}

void putChar(char ch) {
	while (serialIdle() != TRUE);
	outByte(SERIAL_PORT, ch);
}

void putStr(char *ch){
	while(ch && (*ch) && (*ch)!='\0'){
		putChar(*ch);
		ch++;
	}
}

void putNum(uint32_t num){
	char buf[10], *p = &buf[10];
	*--p = 0;
	if (num == 0) {*--p = '0';}
	while (num) {
		if (num % 16 > 9) *--p = (num % 16 - 10) + 'a';
		else *--p = (num % 16) + '0';
		num /= 16;
	}
	while (*p) putChar(*p++);
}

void printk(const char *fmt, ...) {
	uint32_t *args = (uint32_t*)(&fmt) + 1;
	push_off();
	while (*fmt) {
		if (*fmt == '%') {
			++fmt;
			switch (*fmt) {
			case 0: --fmt; break;
			case 'd': putNum(*args++); break;
			case 's': putStr((char*)(*args++)); break;
			case 'c': putChar((char)(*args++)); break;
			default: putChar(*fmt); break;
			}
		} else putChar(*fmt);
		++fmt;
	}
	pop_off();
}
