
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>

int
main(int argc, char *argv[])
{
	struct iovec vec[2];
	char buf1[10];
	char buf2[10];
	int str_len;

	memset(buf1, 0, sizeof(buf1));
	memset(buf2, 0, sizeof(buf2));

	vec[0].iov_base = buf1;
	vec[0].iov_len = 3;

	vec[1].iov_base = buf2;
	vec[1].iov_len = 4;

	str_len = readv(0, vec, 2);
	printf("Read bytes: %d\n", str_len);
	printf("First msg: %s\n", buf1);
	printf("Second msg: %s\n", buf2);

	return 0;
}


