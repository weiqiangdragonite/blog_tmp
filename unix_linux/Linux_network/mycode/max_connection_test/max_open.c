/*
 * 测试最大打开的连接数/文件描述符数
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

int
main(int argc, char *argv[])
{
	int fd, counter;

	counter = 0;
	while (1) {
		fd = open("max_open.c", O_RDONLY);
		if (fd == -1) {
			perror("open() failed");
			exit(-1);
		}

		++counter;
		printf("fd numbers = %d\n", counter);
	}

	return 0;
}
