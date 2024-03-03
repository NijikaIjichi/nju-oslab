#ifndef __SERIAL_H__
#define __SERIAL_H__

void init_serial();
void putchar(char ch);
void serial_handle();
char getchar();
int serial_write(const void *buf, size_t count);
int serial_read(void *buf, size_t count);

#endif
