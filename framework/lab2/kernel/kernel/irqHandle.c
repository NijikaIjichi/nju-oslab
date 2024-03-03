#include "x86.h"
#include "device.h"

extern int displayRow;
extern int displayCol;

extern uint32_t keyBuffer[MAX_KEYBUFFER_SIZE];
extern int bufferHead;
extern int bufferTail;

int tail=0;

void GProtectFaultHandle(struct TrapFrame *tf);

void KeyboardHandle(struct TrapFrame *tf);

void syscallHandle(struct TrapFrame *tf);
void syscallWrite(struct TrapFrame *tf);
void syscallPrint(struct TrapFrame *tf);
void syscallRead(struct TrapFrame *tf);
void syscallGetChar(struct TrapFrame *tf);
void syscallGetStr(struct TrapFrame *tf);


void irqHandle(struct TrapFrame *tf) { // pointer tf = esp
	/*
	 * 中断处理程序
	 */
	/* Reassign segment register */
	asm volatile("movw %%ax, %%ds"::"a"(KSEL(SEG_KDATA)));

	switch(tf->irq) {
		// TODO: 填好中断处理程序的调用



		default:assert(0);
	}
}

void GProtectFaultHandle(struct TrapFrame *tf){
	assert(0);
	return;
}

void KeyboardHandle(struct TrapFrame *tf){
	uint32_t code = getKeyCode();

	if(code == 0xe){ // 退格符
		//  要求只能退格用户键盘输入的字符串，且最多退到当行行首
		if(displayCol>0&&displayCol>tail){
			displayCol--;
			uint16_t data = 0 | (0x0c << 8);
			int pos = (80*displayRow+displayCol)*2;
			asm volatile("movw %0, (%1)"::"r"(data),"r"(pos+0xb8000));
			
		}
	}else if(code == 0x1c){ // 回车符
		// 处理回车情况
		keyBuffer[bufferTail++]='\n';
		displayRow++;
		displayCol=0;
		tail=0;
		if(displayRow==25){
			scrollScreen();
			displayRow=24;
			displayCol=0;
		}
	}else if(code < 0x81){ // 正常字符
		// 注意输入的大小写的实现、不可打印字符的处理
		char character=getChar(code);
		if(character!=0){
			putChar(character);//put char into serial
			uint16_t data=character|(0x0c<<8);
			keyBuffer[bufferTail++]=character;
			bufferTail%=MAX_KEYBUFFER_SIZE;
			int pos=(80*displayRow+displayCol)*2;
			asm volatile("movw %0, (%1)"::"r"(data),"r"(pos+0xb8000));
			displayCol+=1;
			if(displayCol==80){
				displayCol=0;
				displayRow++;
				if(displayRow==25){
					scrollScreen();
					displayRow=24;
					displayCol=0;
				}
			}
		}
	}
	updateCursor(displayRow, displayCol);
	
}

void syscallHandle(struct TrapFrame *tf) {
	switch(tf->eax) { // syscall number
		case 0:
			syscallWrite(tf);
			break; // for SYS_WRITE
		case 1:
			syscallRead(tf);
			break; // for SYS_READ
		default:break;
	}
}

void syscallWrite(struct TrapFrame *tf) {
	switch(tf->ecx) { // file descriptor
		case 0:
			syscallPrint(tf);
			break; // for STD_OUT
		default:break;
	}
}

void syscallPrint(struct TrapFrame *tf) {
	int sel =  USEL(SEG_UDATA);//segment selector for user data, need further modification
	char *str = (char*)tf->edx;
	int size = tf->ebx;
	int i = 0;
	int pos = 0;
	char character = 0;
	uint16_t data = 0;
	asm volatile("movw %0, %%es"::"m"(sel));
	for (i = 0; i < size; i++) {
		asm volatile("movb %%es:(%1), %0":"=r"(character):"r"(str+i));
		//  完成光标的维护和打印到显存

		if(character!='\n'){
			data = character | (0x0c << 8);
			pos = (80*displayRow+displayCol)*2;
			asm volatile("movw %0, (%1)"::"r"(data),"r"(pos+0xb8000));
			displayCol+=1;
			if(displayCol==80){
				displayCol=0;
				displayRow++;
				if(displayRow==25){
					scrollScreen();
					displayRow=24;
					displayCol=0;
				}
			}

		}
		else{
			displayCol=0;
			displayRow++;
			if(displayRow==25){
				scrollScreen();
				displayRow=24;
				displayCol=0;
			}
		}
	}
	tail=displayCol;
	updateCursor(displayRow, displayCol);
}

void syscallRead(struct TrapFrame *tf){
	switch(tf->ecx){ //file descriptor
		case 0:
			syscallGetChar(tf);
			break; // for STD_IN
		case 1:
			syscallGetStr(tf);
			break; // for STD_STR
		default:break;
	}
}

void syscallGetChar(struct TrapFrame *tf){

	bufferHead=0;
	bufferTail=0;
	keyBuffer[bufferHead]=0;
	keyBuffer[bufferHead+1]=0;
	char c=0;
	
	while(c == 0){
		enableInterrupt();
		c = keyBuffer[bufferHead];
		putChar(c);
		disableInterrupt();
	}
	tf->eax=c;

	char wait=0;
	while(wait==0){
		enableInterrupt();
		wait = keyBuffer[bufferHead+1];
		disableInterrupt();
	}
	return;
}

void syscallGetStr(struct TrapFrame *tf){
	char* str=(char*)(tf->edx);//str pointer
	int size=(int)(tf->ebx);//str size
	bufferHead=0;
	bufferTail=0;
	for(int j=0;j<MAX_KEYBUFFER_SIZE;j++)keyBuffer[j]=0;//init
	int i=0;

	char tpc=0;
	while(tpc!='\n' && i<size){

		while(keyBuffer[i]==0){
			enableInterrupt();
		}
		tpc=keyBuffer[i];
		i++;
		disableInterrupt();
	}

	int selector=USEL(SEG_UDATA);
	asm volatile("movw %0, %%es"::"m"(selector));
	int k=0;
	for(int p=bufferHead;p<i-1;p++){
		asm volatile("movb %0, %%es:(%1)"::"r"(keyBuffer[p]),"r"(str+k));
		k++;
	}
	asm volatile("movb $0x00, %%es:(%0)"::"r"(str+i));
	return;
}
