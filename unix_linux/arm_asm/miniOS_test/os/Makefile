#
# Makefile template:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = miniOS

# space-separated list of source files
SRC_S = 
SRC_C = 

# automatically generated list of object files
OBJS = *.o
OBJS_DIR = $(shell pwd)/os_obj

# compiler to use
CC = arm-none-linux-gnueabi-gcc
LD = arm-none-linux-gnueabi-ld
CP = arm-none-linux-gnueabi-objcopy
DP = arm-none-linux-gnueabi-objdump

#include path
INCLUDE = -I$(shell pwd)/include

# lib path
LIB = -L$(shell pwd)/os_obj

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib -fomit-frame-pointer $(INCLUDE) -c

# default target
$(EXE).bin : $(OBJS)
	cp $(EXE).lds $(OBJS_DIR); cd $(OBJS_DIR); \
	$(LD) -T $(EXE).lds -o $(EXE).elf *.o; \
	cp $(EXE).elf ../
	$(CP) -O binary $(EXE).elf $@
	$(DP) -D $(EXE).elf > $(EXE).dis
	rm -rf $(OBJS_DIR)

# dependencies
*.o :
	rm -rf $(OBJS_DIR); mkdir $(OBJS_DIR)
	cd os_asm; make
	cd os_c; make

#%.o : %.s
#	$(CC) $(CFLAGS) $^ -o $@

#%.o : %.c
#	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	cd os_c; make clean
	cd os_asm; make clean
	rm -f core *.bin *.elf *.dis; rm -rf $(OBJS_DIR)