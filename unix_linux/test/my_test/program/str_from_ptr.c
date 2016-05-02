#include <stdio.h>
#include <unistd.h>
#include <math.h>
// Link with -lm


long htolong(const unsigned char *src, ssize_t len);
char *hex_to_str(unsigned char *src, char *dest, ssize_t len);
char *write_to_str(unsigned char hex, char *dest);

int
main(int argc, char *argv[])
{
	unsigned char buf[1024];

/*
	int *card_id;

	buf[32] = 0x00;
	buf[33] = 0x00;
	buf[34] = 0x00; // 4d
	buf[35] = 0x4d; // fb
	buf[36] = 0x50; // 50

	//int num = 1;
	//unsigned char *ptr = (unsigned char *) &num;
	//printf("num = %d; %02x %02x %02x %02x\n", num, *ptr, *++ptr, *++ptr, *++ptr);
	long card = htolong(&buf[32], 5);
	

	//snprintf(card_id, sizeof(card_id), "%d", (int) &buf[32]);
	//card_id = (int *) &buf[33];

	printf("card = %ld\n", card);
*/


	// 14 07 30 21 36 56
	buf[37] = 0x14;
	buf[38] = 0x07;
	buf[39] = 0x30;
	buf[40] = 0x21;
	buf[41] = 0x36;
	buf[42] = 0x56;

	char time[13] = { '0' };
	hex_to_str(&buf[37], time, 6);
	printf("time - %s\n", time);

	char datetime[] = "2014-01-01 00:00:00";
	datetime[2] = time[0];
	datetime[3] = time[1];
	datetime[5] = time[2];
	datetime[6] = time[3];
	datetime[8] = time[4];
	datetime[9] = time[5];
	datetime[11] = time[6];
	datetime[12] = time[7];
	datetime[14] = time[8];
	datetime[15] = time[9];
	datetime[17] = time[10];
	datetime[18] = time[11];

	printf("datetime = %s\n", datetime);


	return 0;
}


long
htolong(const unsigned char *src, ssize_t len)
{
	int tmp;
	//int i;
	long sum = 0;

	for (; len > 0; --len) {
		tmp = (int) *src++;
		sum += tmp * pow(16, 2 * len - 2);
		printf("len = %d, tmp = %d, sum = %ld\n", len, tmp, sum);
	}
	
	return sum;
}


char *
hex_to_str(unsigned char *src, char *dest, ssize_t len)
{
	unsigned char *d = dest;
	unsigned char *s = src;
	for (; len > 0; --len, s++) {
		printf("%02x - %x %x\n", *s, *s >> 4, *s & 0x0f);
		write_to_str((*s) >> 4, d++);
		write_to_str((*s) & 0x0f, d++);
	}

	return dest;
}


char *
write_to_str(unsigned char hex, char *dest)
{
	switch (hex) {
	case 0x00:
		*dest = '0';
		break;
	case 0x01:
		*dest = '1';
		break;
	case 0x02:
		*dest = '2';
		break;
	case 0x03:
		*dest = '3';
		break;
	case 0x04:
		*dest = '4';
		break;
	case 0x05:
		*dest = '5';
		break;
	case 0x06:
		*dest = '6';
		break;
	case 0x07:
		*dest = '7';
		break;
	case 0x08:
		*dest = '8';
		break;
	case 0x09:
		*dest = '9';
		break;
	case 0x0a:
		*dest = 'A';
		break;
	case 0x0b:
		*dest = 'B';
		break;
	case 0x0c:
		*dest = 'C';
		break;
	case 0x0d:
		*dest = 'D';
		break;
	case 0x0e:
		*dest = 'E';
		break;
	case 0x0f:
		*dest = 'F';
		break;
	default:
		*dest = '?';
		break;
	}

	return dest;
}
