#include "x86.h"
#include "device.h"

#define WIDTH        80
#define HEIGHT       25
#define VGA          ((uint16_t*)(0xb8000))
#define CURR         VGA[displayRow * WIDTH + displayCol]
#define CH_TO_16(ch) (((ch) & 0xff) | (0x0c << 8))

void clearScreen();
void updateCursor(int row, int col);
void scrollScreen();

int displayRow = 0;
int displayCol = 0;
uint16_t displayMem[WIDTH * HEIGHT];
int displayClear = 0;

void initVga() {
  displayRow = 0;
  displayCol = 0;
  displayClear = 0;
  clearScreen();
  updateCursor(0, 0);
}

void clearScreen() {
  int i;
  for (i = 0; i < WIDTH * HEIGHT; i++) {
    VGA[i] = CH_TO_16(0);
  }
}

void updateCursor(int row, int col){
  int cursorPos = row * WIDTH + col;
  outByte(0x3d4, 0x0f);
  outByte(0x3d5, (unsigned char)(cursorPos & 0xff));
  outByte(0x3d4, 0x0e);
  outByte(0x3d5, (unsigned char)((cursorPos>>8) & 0xff));
}

void scrollScreen() {
  int i;
  for (i = 0; i < WIDTH * HEIGHT; i++) {
    displayMem[i] = VGA[i];
  }
  for (i = 0; i < WIDTH * (HEIGHT - 1); i++) {
    VGA[i] = displayMem[i + WIDTH];
  }
  for (; i < WIDTH * HEIGHT; i++) {
    VGA[i] = CH_TO_16(0);
  }
}

void putch(char ch) {
  if (ch == '\n') {
    displayCol = 0;
    displayRow++;
  } else if (ch == '\b') {
    if (displayCol > 0) {
      displayCol--;
      CURR = CH_TO_16(0);
    }
  } else {
    CURR = CH_TO_16(ch);
    if (displayCol < WIDTH - 1) {
      displayCol++;
    } else {
      displayCol = 0;
      displayRow++;
    }
  }
  if (displayRow == HEIGHT) {
    scrollScreen();
    displayRow = HEIGHT - 1;
    displayCol = 0;
  }
  updateCursor(displayRow, displayCol);
}
