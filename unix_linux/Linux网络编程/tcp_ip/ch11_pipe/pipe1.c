
#include <stdio.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	int fds[2];
	char str[] = "hello world!";
	char buf[1024];
	pid_t pid;

	pipe(fds);
	pid = fork();
	if (pid == 0) {
		write(fds[1], str, sizeof(str));
	} else {
		read(fds[0], buf, sizeof(buf));
		printf("[recv] %s\n", buf);
	}

	return 0;
}

