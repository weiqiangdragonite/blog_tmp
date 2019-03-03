
CC      = arm-linux-gnueabi-gcc
LD      = arm-linux-gnueabi-ld
AR      = arm-linux-gnueabi-ar
OBJCOPY = arm-linux-gnueabi-objcopy
OBJDUMP = arm-linux-gnueabi-objdump

INCLUDEDIR 	:= $(shell pwd)/include
CFLAGS 		:= -Wall -Os -fno-builtin-printf -fno-builtin-putc -fno-builtin-putchar
CPPFLAGS   	:= -nostdinc -I$(INCLUDEDIR)

export 	CC AR LD OBJCOPY OBJDUMP INCLUDEDIR CFLAGS CPPFLAGS 

objs := start.o clock.o sdram.o nand.o main.o uart.o lib/libc.a

uart.bin: $(objs)
	${LD} -Tuart.lds -o uart.elf $^
	${OBJCOPY} -O binary -S uart.elf $@
	${OBJDUMP} -D uart.elf > uart.dis

.PHONY : lib/libc.a
lib/libc.a:
	cd lib; make; cd ..
	
%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	make  clean -C lib
	rm -f uart.bin uart.elf uart.dis *.o
	
