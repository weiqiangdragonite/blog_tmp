
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
	char buf[] = "hello, world!\n";

	/* create write only file, if file is exist, clean file data and open */
	fd = open("data.txt", O_CREAT | O_WRONLY | O_TRUNC);
	if (fd == -1) {
		perror("open() failed");
		exit(EXIT_FAILURE);
	}
	printf("file descriptor %d\n", fd);

	if (write(fd, buf, sizeof(buf)) == -1)
		perror("write() failed");

	close(fd);

	return 0;
}

