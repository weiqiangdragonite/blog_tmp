
#
u-boot.bin : start.o
	arm-linux-gnueabi-ld -Ttext 0 start.o -o buz.elf
	arm-linux-gnueabi-objcopy -O binary buz.elf u-boot.bin
	arm-linux-gnueabi-objdump -D buz.elf > buz.dis

start.o : start.s
	arm-linux-gnueabi-gcc -c start.s -o start.o

clean:
	rm -rf start.o buz.elf u-boot.bin buz.dis
