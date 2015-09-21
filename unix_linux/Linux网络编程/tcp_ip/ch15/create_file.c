
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


#define BUF_SIZE	1024

int
main(int argc, char *argv[])
{
	int fd, size, i, unit, a, b;
	char buf[BUF_SIZE];
	char ch = 0;

	if (argc != 3 && argc != 4) {
		fprintf(stderr, "Usage: %s <filename> <size> [unit]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	memset(buf, 0, sizeof(buf));

	fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC);
	if (fd == -1) {
		perror("open() failed");
		exit(EXIT_FAILURE);
	}

	if (argc == 4) {
		switch ((int) argv[3][0]) {
		case 'k':
		case 'K':
			unit = 1 << 10;
			break;
		case 'm':
		case 'M':
			unit = 1 << 20;
			break;
		case 'g':
		case 'G':
			unit = 1 << 30;
			break;
		case 'b':
		case 'B':
		default:
			unit = 1;
			break;
		}
	}

	size = atoi(argv[2]) * unit;
	printf("size = %d\n", size);

	a = size / 1024;
	b = size % 1024;

	for (i = 0; i < a; ++i)
		write(fd, buf, sizeof(buf));
	for (i = 0; i < b; ++i)
		write(fd, &ch, sizeof(ch));

	close(fd);
	return 0;
}

