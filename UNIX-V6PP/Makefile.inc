#�����
CC = g++
LD = ld
OBJCOPY = objcopy
OBJDUMP = objdump
ASM = nasm
NM = nm
MAKE = make

#��������
#g++����
CFLAGS = -Wall -O0 -g -nostartfiles -nostdlib -fno-builtin -fno-rtti -fno-exceptions -nostdinc
#ld����
LDFLAGS = -T $(LINKSCRIPT)

#Ŀ¼����
OBJFILES = ..\objs
TARGET = ..\targets\objs
DIRS = boot kernel machine interrupt mm proc dev fs tty test pe lib program shell
INCLUDE = ..\include
IMGTARGET = ..\targets\img
#"++"����������ţ������Ҫ������
UNIXV6PPTARGET = "..\targets\UNIXV6++"
MakeImageDebug = ..\tools\MakeImage\bin\Debug

#�ļ�����
LINKSCRIPT = Link.ld
SECTOR2_OBJ = sector2.bin
CPP_OBJS = $(foreach OBJFILE, \
	$(shell dir $(TARGET)\*.o /B), \
	$(TARGET)\$(OBJFILE) \
	)
OBJS = $(TARGET)\$(SECTOR2_OBJ) $(CPP_OBJS)
IMG = c.img

#�������
.c.s:
	$(CC) $(CFLAGS) \
	-S -o $*.s $<
.s.o:
	$(AS) -o $*.o $<
.c.o:
	$(CC) $(CFLAGS) \
	-c -o $*.o $<