/*
 * base64.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static const char base64_set[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char base64_pad = '=';


/*
 *
 */
char *
base64_encode(const char *src, size_t src_len, char *dest, size_t dest_len)
{
	unsigned char input[3];
	unsigned char output[4];
	char *s = (char *) src;
	char *d = dest;
	char *data = NULL;
	int padding = 0;


	while (src_len > 0) {
		memset(input, 0, sizeof(input));

		/** get three bytes to input */
		if (src_len >= 3) {
			memcpy((void *) input, (void *) s, 3);
			src_len -= 3;
			s += 3;
		} else {
			memcpy(input, s, src_len);
			padding = src_len;
			src_len -= src_len;
			s += src_len;
		}

		//printf("%d - %d - %d\n", input[0], input[1], input[2]);


		/** convert into four bytes output */
		output[0] = input[0] >> 2;
		output[1] = ((input[0] & 0x03) << 4) | (input[1] >> 4);
		output[2] = ((input[1] & 0x0F) << 2) | (input[2] >> 6);
		output[3] = input[2] & 0x3F;

		//printf("%d - %d - %d - %d\n",
		//	output[0], output[1], output[2], output[3]);


		/** encode */
		output[0] = base64_set[output[0]];
		output[1] = base64_set[output[1]];
		output[2] = base64_set[output[2]];
		output[3] = base64_set[output[3]];

		/* padding == 0 -->        have 3 bytes src */
		if (padding == 2)	/* have 2 bytes src */
			output[3] = base64_pad;
		else if (padding == 1)	/* have 1 byte src */
			output[2] = output[3] = base64_pad;

		//printf("%d : %c %c %c %c\n", padding,
		//	output[0], output[1], output[2], output[3]);


		/** add to dest string */
		/* if dest len is smaller, make sure end with '\0' */
		data = (char *) output;
		padding = 4;
		while (dest_len > 0 && padding > 0) {
			*d++ = *data++;
			--dest_len;
			--padding;
		}
		if (dest_len == 0) {
			--d;
			break;
		}
	}
	*d = '\0';

	return dest;
}




/*
 * src_len % 4 == 0
 */
char *
base64_decode(const char *b64src, size_t src_len, char *dest, size_t dest_len)
{
	unsigned char input[4];
	unsigned char output[3];
	char *s = (char *) b64src;
	char *d = dest;
	char *data = NULL;
	int count = 0;

	while (src_len > 0) {
		/** get four bytes into input */
		if (src_len >= 4) {
			memcpy((void *) input, (void *) s, 4);
			src_len -= 4;
			s += 4;
		} else {
			*dest = '\0';
			return NULL;
		}


		/** decode, find the position in the base64 set */
		input[0] = strchr(base64_set, input[0]) - base64_set;
		input[1] = strchr(base64_set, input[1]) - base64_set;
		if (input[2] == base64_pad)
			input[2] = 0;
		else
			input[2] = strchr(base64_set, input[2]) - base64_set;
		if (input[3] == base64_pad)
			input[3] = 0;
		else
			input[3] = strchr(base64_set, input[3]) - base64_set;

		//printf("%d %d %d %d\n",
		//	input[0], input[1], input[2], input[3]);


		/** convert into three bytes output */
		output[0] = (input[0] << 2) | (input[1] >> 4);
		output[1] = (input[1] << 4) | (input[2] >> 2);
		output[2] = (input[2] << 6) | input[3];

		//printf("%d - %d - %d\n", output[0], output[1], output[2]);


		/** add to dest string */
		count = 3;
		data = (char *) output;
		while (dest_len > 0 && count > 0) {
			*d++ = *data++;
			--dest_len;
			--count;
		}
		if (dest_len == 0) {
			--d;
			break;
		}
	}
	*d = '\0';

	return dest;
}



/*
int
main(void)
{
	char *s = "hello, world!";
	char buf[1024];
	base64_encode(s, strlen(s), buf, 1024);
	printf("encode:\n%s\n", buf);

	base64_decode(buf, strlen(buf), buf, 1024);
	printf("decode:\n%s\n", buf);

	return 0;
}
*/
