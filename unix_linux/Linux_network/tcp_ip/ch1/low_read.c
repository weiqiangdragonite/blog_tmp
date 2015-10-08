
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int
main(int argc, char *argv[])
{
	int fd;
	char buf[128];

	fd = open("data.txt", O_RDONLY);
	if (fd == -1) {
		perror("open() failed");
		exit(EXIT_FAILURE);
	}
	printf("file descriptor %d\n", fd);

	if (read(fd, buf, sizeof(buf)) == -1)
		perror("write() failed");
	else
		printf("file data: %s", buf);
	close(fd);

	return 0;
}

