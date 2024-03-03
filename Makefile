default: all

QEMU = qemu-system-i386

.PHONY: clean all

utils/mkfs: utils/mkfs.c
	gcc utils/mkfs.c -o utils/mkfs -O2 -Wall -Werror

os.img: utils/mkfs
	@cd bootloader; make
	@cd user; make
	@cd kernel; make
	cat bootloader/bootloader.bin kernel/kMain.elf > os.img
	./utils/mkfs os.img disk

all: os.img

play: os.img
	$(QEMU) -serial stdio os.img

debug: os.img
	$(QEMU) -serial stdio -s -S os.img

log: os.img
	$(QEMU) -serial stdio os.img -d int,cpu_reset -D qemu.log

clean:
	@cd bootloader; make clean
	@cd kernel; make clean
	@cd user; make clean
	rm -f os.img utils/mkfs
