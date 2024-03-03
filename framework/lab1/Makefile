os.img:
	cd bootloader; make bootloader.bin
	cd app; make app.bin
	cat bootloader/bootloader.bin app/app.bin > os.img

clean:
	cd bootloader; make clean
	cd app; make clean
	rm -f os.img

qemu:
	qemu-system-i386 os.img

qemu-gdb:
	qemu-system-i386 -s -S os.img

qemu-nox-gdb:
	qemu-system-i386 -nographic -s -S os.img

gdb:
	gdb -n -x ./.gdbconf/.gdbinit
