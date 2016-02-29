
#include <stdio.h>
#include <sys/uio.h>

int
main(int argc, char *argv[])
{
	struct iovec vec[2];
	char buf1[] = "1234567890";
	char buf2[] = "ABCDEFG";
	int str_len;

	vec[0].iov_base = buf1;
	vec[0].iov_len = 3;

	vec[1].iov_base = buf2;
	vec[1].iov_len = 4);

	str_len = writev(1, vec, 2);
	printf("\n");
	printf("Write bytes: %d\n", str_len);

	return 0;
}


