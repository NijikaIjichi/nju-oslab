# General

default: all

.PHONY: clean clean-all clean-fs all qemu qemu-gdb gdb print-gdbport grade submit pack

# REMEMBER TO MAKE CLEAN AFTER CHANGE ME!
STAGE  := phase6
STAGES := phase1 phase2 phase3 phase4 phase5 phase6

ifeq ($(filter $(STAGES), $(STAGE)), ) # STAGE must be valid
$(error Invalid STAGE. Supported: $(STAGES))
endif

OBJDIR := build
IMAGE  := $(OBJDIR)/os.img
QEMU   := qemu-system-i386
CC     := gcc
LD     := ld

CFLAGS := -O1 -std=gnu11 -ggdb3 -Wall -Werror -fno-asynchronous-unwind-tables -fno-builtin -fno-stack-protector -Wno-main -ffreestanding -m32 -fno-pic -fno-omit-frame-pointer -march=i386 -fcf-protection=none
ASFLAGS := -m32 -fno-pic
LDFLAGS := -m elf_i386
QEMU_FLAGS := -no-reboot -serial stdio -display none#-nographic

all: $(IMAGE)

clean:
	rm -rf $(OBJDIR)/boot $(OBJDIR)/kernel $(OBJDIR)/user $(OBJDIR)/utils $(OBJDIR)/*.img $(OBJDIR)/*.backup

clean-all:
	rm -rf $(OBJDIR)

qemu: $(IMAGE)
	$(QEMU) $(IMAGE) $(QEMU_FLAGS)

qemu-log: $(IMAGE)
	$(QEMU) $(IMAGE) $(QEMU_FLAGS) -d int,cpu_reset -D qemu.log

qemu-gdb: $(IMAGE)
	$(QEMU) $(IMAGE) $(QEMU_FLAGS) -s -S

gdb:
	gdb -n -x ./.gdbconf/.gdbinit

print-gdbport:
	@echo 1234

grade:
	grade/grade-$(STAGE)

submit:
	python3 ok --config okconfig/$(STAGE).ok --submit

pack:
	mkdir -p build/submit
	python3 ok --config okconfig/$(STAGE).ok --pack build/submit

# Bootloader

BOOT_CSRCS := $(shell find boot -name "*.c")
BOOT_SSRCS := $(shell find boot -name "*.S")
BOOT_COBJS := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(BOOT_CSRCS))))
BOOT_SOBJS := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(BOOT_SSRCS))))
BOOT_ELF   := $(OBJDIR)/boot/boot
BOOT_IMG   := $(OBJDIR)/boot/boot.img

$(BOOT_COBJS): $(OBJDIR)/%.o: %.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) $< -o $@

$(BOOT_SOBJS): $(OBJDIR)/%.o: %.S
	@echo + AS $<
	@mkdir -p $(dir $@)
	@$(CC) -c $(ASFLAGS) $< -o $@

$(BOOT_ELF): $(BOOT_SOBJS) $(BOOT_COBJS)
	@echo LD "->" $@
	@$(LD) $(LDFLAGS) -e _start -Ttext 0x7c00 $(BOOT_SOBJS) $(BOOT_COBJS) -o $@

$(BOOT_IMG): $(BOOT_ELF)
	@echo CREATE "->" $@
	@objcopy -O binary -j .text $(BOOT_ELF) $(BOOT_IMG).tmp
	@utils/genboot.pl $(BOOT_IMG).tmp $(BOOT_IMG)

# Lib

LIB_SRCS := $(shell find lib -name "*.c")
LIB_OBJS := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(LIB_SRCS))))
LIB_ARCH := $(OBJDIR)/lib/lib.a
LIB_INC  := lib/include

$(LIB_OBJS): $(OBJDIR)/%.o: %.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -I $(LIB_INC) $< -o $@

$(LIB_ARCH): $(LIB_OBJS)
	@echo AR "->" $@
	@ar rcs $(LIB_ARCH) $(LIB_OBJS)

# Kernel

KERN_CSRCS := $(shell find kernel -name "*.c")
KERN_SSRCS := $(shell find kernel -name "*.S")
KERN_COBJS := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(KERN_CSRCS))))
KERN_SOBJS := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(KERN_SSRCS))))
KERN_ELF   := $(OBJDIR)/kernel/kernel
KERN_IMG   := $(OBJDIR)/kernel/kernel.img
KERN_INC   := kernel/include

$(KERN_COBJS): $(OBJDIR)/%.o: %.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -I $(LIB_INC) -I $(KERN_INC) $< -o $@

$(KERN_SOBJS): $(OBJDIR)/%.o: %.S
	@echo + AS $<
	@mkdir -p $(dir $@)
	@$(CC) -c $(ASFLAGS) -I $(KERN_INC) $< -o $@

$(KERN_ELF): $(KERN_COBJS) $(KERN_SOBJS) $(LIB_ARCH)
	@echo LD "->" $@
	@$(LD) $(LDFLAGS) -e main -Ttext 0x101000 $(KERN_COBJS) $(KERN_SOBJS) $(LIB_ARCH) -o $@

$(KERN_IMG): $(KERN_ELF)
	@echo CREATE "->" $@
	@strip -s $(KERN_ELF) -o $(KERN_ELF).tmp
	@utils/genkern.pl $(KERN_ELF).tmp $(KERN_IMG)

# User

USER_LIBSRC := $(shell find user/ulib -name "*.c")
USER_LIBOBJ := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(USER_LIBSRC))))
USER_SRCS   := $(shell find user/src/$(STAGE) -name "*.c")
USER_OBJS   := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(USER_SRCS))))
USER_ELFS   := $(addprefix $(OBJDIR)/, $(basename $(USER_SRCS)))
USER_INC    := user/include
USER_DISK   := $(OBJDIR)/user/user.img

ifeq ($(STAGE), phase1)
USER_ADDR   := 0x1001000
else
USER_ADDR   := 0x8048000
endif

$(USER_LIBOBJ): $(OBJDIR)/%.o: %.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -I $(LIB_INC) -I $(USER_INC) $< -o $@

$(USER_OBJS): $(OBJDIR)/%.o: %.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) -c $(CFLAGS) -I $(LIB_INC) -I $(USER_INC) $< -o $@.tmp
	@objcopy -R .note -R .comment -R .note.gnu.property $@.tmp $@

$(USER_ELFS): $(OBJDIR)/%: $(OBJDIR)/%.o $(USER_LIBOBJ) $(LIB_ARCH)
	@echo LD "->" $@
	@$(LD) $(LDFLAGS) -e _start -Ttext $(USER_ADDR) $< $(USER_LIBOBJ) $(LIB_ARCH) -o $@

ifeq ($(STAGE), phase6)
USER_GENC   := utils/mkfs.c
USER_GEN    := $(OBJDIR)/utils/mkfs
else
USER_GENC   := utils/genuser.c
USER_GEN    := $(OBJDIR)/utils/genuser
endif

ifeq ($(filter phase5 phase6, $(STAGE)), $(STAGE))
USER_FILE   := $(shell find user/file -type f)
endif

$(USER_GEN): $(USER_GENC)
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) -O1 -std=gnu11 -ggdb -Wall -Werror $< -o $@

$(USER_DISK): $(USER_ELFS) $(USER_GEN) $(USER_FILE)
	@echo CREATE "->" $@
	@$(USER_GEN) $(USER_DISK) $(USER_ELFS) $(USER_FILE)

clean-fs:
	rm -rf $(USER_DISK) $(IMAGE)

# Image

$(IMAGE): $(BOOT_IMG) $(KERN_IMG) $(USER_DISK)
	@echo CREATE "->" $@
	@cat $(BOOT_IMG) $(KERN_IMG) $(USER_DISK) > $(IMAGE)
