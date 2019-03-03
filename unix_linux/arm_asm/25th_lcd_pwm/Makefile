
CC      = arm-linux-gcc
LD      = arm-linux-ld
AR      = arm-linux-ar
OBJCOPY = arm-linux-objcopy
OBJDUMP = arm-linux-objdump

INCLUDEDIR 	:= $(shell pwd)/include
CFLAGS 		:= -Wall -Os -fno-builtin-printf -fno-builtin-putc -fno-builtin-putchar
CPPFLAGS   	:= -nostdinc -I$(INCLUDEDIR)
LIBPATH   	:= -lgcc -L/usr/local/arm/4.2.2-eabi/usr/lib/gcc/arm-unknown-linux-gnueabi/4.2.2

export 	CC AR LD OBJCOPY OBJDUMP INCLUDEDIR CFLAGS CPPFLAGS 

objs := start.o clock.o sdram.o nand.o main.o irq.o uart.o lcd.o framebuffer.o font_8x8.o pwm.o lib/libc.a

uart.bin: $(objs)
	${LD} -Tuart.lds -o uart.elf $^ ${LIBPATH}
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
	
