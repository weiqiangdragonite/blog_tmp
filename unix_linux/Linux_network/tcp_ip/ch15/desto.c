
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


int
main(int argc, char *argv[])
{
	FILE *fp;
	int fd;

	char buf[] = "hello, world\n";
	fd = open("data.dat", O_WRONLY | O_CREAT | O_TRUNC);
	write(fd, buf, strlen(buf));

	fp = fdopen(fd, "w");
	fputs("tcp/ip\n", fp);

	write(fd, buf, strlen(buf));
	fputs("hahaha", fp);

	fclose(fp);

	return 0;
}

