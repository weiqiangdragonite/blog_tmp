
#include <stdio.h>

unsigned short
crc16(const unsigned char *ptr, int length)
{
	unsigned short crc = 0xFFFF;
	int i;

	while (length--) {
		crc ^= *ptr++;
		for (i = 0; i < 8; ++i) {
			if (crc & 0x01) {
				crc >>= 1;
				crc ^= 0xA001;
			} else
				crc >>= 1;
		}
	}

	return crc;
}


int
main(void)
{
	unsigned char cmd[8];
	cmd[0] = 0x00;
	cmd[1] = 0x04;
	cmd[2] = 0x00;
	cmd[3] = 0x00;
	cmd[4] = 0x00;
	cmd[5] = 0x01;

	unsigned short crc;
	char *ptr;
	crc = crc16(cmd, 6);
	ptr = (char *) &crc;
	cmd[6] = *ptr;
	cmd[7] = *++ptr;

	int i;
	for (i = 0; i < 8; ++i) {
		printf("cmd[%d] = 0x%02x\n", i, cmd[i]);
	}
}
