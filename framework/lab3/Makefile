QEMU = qemu-system-i386

os.img:
	@cd bootloader; make
	@cd kernel; make
	@cd app; make
	@cd app_print; make
	cat bootloader/bootloader.bin kernel/kMain.elf app/uMain.elf app_print/app_print.elf > os.img
	#   bootloader从0扇区开始，kernel从1扇区开始，app1从201扇区开始，app2从221扇区开始。注意阅读各种makefile！！！
play: os.img
	$(QEMU) -serial stdio os.img

debug: os.img
	$(QEMU) -serial stdio -s -S os.img
gdb:
	gdb -n -x ./.gdbconf/.gdbinit



clean:
	@cd bootloader; make clean
	@cd kernel; make clean
	@cd app; make clean
	@cd app_print; make clean
	rm -f os.img
