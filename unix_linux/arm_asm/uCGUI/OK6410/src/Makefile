#
# Makefile:
#
# By weiqiangdragonite@gmail.com
#

# name for executable
EXE = ok6410-uCOS-II

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
GCC_LIB = -L/usr/local/arm/arm-2013.05/lib/gcc/arm-none-linux-gnueabi/4.7.3

#include path
INCLUDE = 

# lib path
LIB = -L$(shell pwd)/os_obj

# flags to pass compiler
CFLAGS = -std=c99 -Wall -Werror -nostdlib -fomit-frame-pointer $(INCLUDE) -c

# default target
$(EXE).bin : $(OBJS)
	cp $(EXE).lds $(OBJS_DIR); cd $(OBJS_DIR); \
	$(LD) -T $(EXE).lds -o $(EXE).elf *.o uCGUI.a $(GCC_LIB) -lgcc; \
	cp $(EXE).elf ../
	$(CP) -O binary $(EXE).elf $@
	$(DP) -D $(EXE).elf > $(EXE).dis
	rm -rf $(OBJS_DIR)

# dependencies
*.o :
	rm -rf $(OBJS_DIR); mkdir $(OBJS_DIR)
	cd bsp; make
	cd arm; make
	cd kernel; make
	cd uCGUI; make
	cd app; make

#%.o : %.s
#	$(CC) $(CFLAGS) $^ -o $@

#%.o : %.c
#	$(CC) $(CFLAGS) $^ -o $@

# housekeeping
.PHONY : clean
clean:
	cd bsp; make clean
	cd arm; make clean
	cd kernel; make clean
	cd app; make clean
	cd uCGUI; make clean
	rm -f core *.bin *.elf *.dis; rm -rf $(OBJS_DIR)