
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define BUF_SIZE	3


int
main(int argc, char *argv[])
{
	int fd1, fd2;
	int len;
	char buf[BUF_SIZE];
	clock_t start, stop;
	double duration;

	fd1 = open("news.txt", O_RDONLY);
	fd2 = open("copy.txt", O_WRONLY | O_CREAT | O_TRUNC);

	start = clock();
	while ((len = read(fd1, buf, sizeof(buf))) > 0)
		write(fd2, buf, len);
	stop = clock();
	duration = (double) (stop - start) / CLOCKS_PER_SEC;

	printf("used time [3]: %f\n", duration);

	close(fd1);
	close(fd2);

	return 0;
}

