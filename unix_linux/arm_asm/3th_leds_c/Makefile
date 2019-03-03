CFLAGS = -std=c99 -Wall -Werror -nostdlib -fomit-frame-pointer -O2 -c



led.bin: start.o led.o
	arm-none-linux-gnueabi-ld -Ttext 0 -o led.elf start.o led.o
	arm-none-linux-gnueabi-objcopy -O binary led.elf led.bin
	arm-none-linux-gnueabi-objdump -D led.elf > led.dis

start.o : start.S
	arm-none-linux-gnueabi-gcc $(CFLAGS) $^ -o $@
led.o : led.c
	arm-none-linux-gnueabi-gcc $(CFLAGS) $^ -o $@

clean:
	rm *.o led.elf led.bin led.dis
