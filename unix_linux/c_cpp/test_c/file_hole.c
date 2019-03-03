#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

char *buf1 = "abcdefg";
char *buf2 = "ABCDEFG";

int
main(void)
{
	int	fd;

	if ((fd = open("file.hole", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1) {
		fprintf(stderr, "create file error\n");
		exit(EXIT_FAILURE);
	}

	if (write(fd, buf1, sizeof(buf1)) != sizeof(buf1)) {
		fprintf(stderr, "write buf1 error\n");
		exit(EXIT_FAILURE);
	}

	if (lseek(fd, 16384, SEEK_SET) == -1) {
		fprintf(stderr, "lseek error\n");
		exit(EXIT_FAILURE);
	}

	if (write(fd, buf2, sizeof(buf2)) != sizeof(buf2)) {
		fprintf(stderr, "write buf2 error\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
