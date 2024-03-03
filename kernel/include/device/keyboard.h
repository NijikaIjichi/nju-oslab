#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#define MAX_KEYBUFFER_SIZE 256

void initKeyTable();

uint32_t getKeyCode();

char getChar(uint32_t code);

void key_buf_push_back(char ch);
char key_buf_pop_back();
char key_buf_pop_front();

extern uint32_t key_buf_lock;


#endif