/*
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void)
{
	int fd;
	printf("process uid = [%d], euid = [%d]", getuid(), geteuid());

	if ( (fd = open("/tmp/test.txt", O_RDWR)) == -1 )
	{
		fprintf(stderr, "open failed, error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("open success\n");
	close(fd);

	return 0;
}
