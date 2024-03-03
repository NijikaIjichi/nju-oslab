default: all

CC := gcc
LD := ld

CFLAGS := -m32 -march=i386 -static \
	 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer \
	 -Wall -Werror -O2 -I../lib -g -fno-pic
LDFLAGS := -m elf_i386

UCFILES := $(shell find ./ -name "*.c")
LCFILES := $(shell find ../lib -name "*.c")
LSFILES := $(shell find ../lib -name "*.S")
UOBJS := $(UCFILES:.c=.o)
LOBJS := $(LCFILES:.c=.o) $(LSFILES:.S=.o)
UELFS := $(basename $(UOBJS))
#TARGET := _user_apps
#HEADER := ../kernel/include/files.h
#TARGET_GEN := ../utils/gen_user.py

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(UELFS): %: %.o $(LOBJS)
	$(LD) $(LDFLAGS) -o $@ $^

#$(TARGET): $(UELFS)
#	python3 $(TARGET_GEN) $(TARGET) $(HEADER) $(UELFS)

.PHONY: all clean

all: $(UELFS)
	cp $(UELFS) ../disk

clean:
	rm -rf $(UOBJS) $(LOBJS) $(UELFS)
	@#touch $(HEADER)
