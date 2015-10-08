
#include <stdio.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	int fds[2];
	char str1[] = "hello world!";
	char str2[] = "thank you!";
	char buf[1024];
	pid_t pid;

	pipe(fds);
	pid = fork();
	if (pid == 0) {
		write(fds[1], str1, sizeof(str1));
		sleep(2);
		read(fds[0], buf, sizeof(buf));
		printf("child recv: %s\n", buf);
	} else {
		read(fds[0], buf, sizeof(buf));
		printf("parent recv: %s\n", buf);
		write(fds[1], str2, sizeof(str2));
		sleep(3);
	}

	return 0;
}

