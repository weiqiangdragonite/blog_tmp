
#include <stdio.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	int fds1[2], fds2[2];
	char str1[] = "hello world!";
	char str2[] = "thank you!";
	char buf[1024];
	pid_t pid;

	pipe(fds1);
	pipe(fds2);

	pid = fork();
	if (pid == 0) {
		write(fds1[1], str1, sizeof(str1));
		read(fds2[0], buf, sizeof(buf));
		printf("child recv: %s\n", buf);
	} else {
		read(fds1[0], buf, sizeof(buf));
		printf("parent recv: %s\n", buf);
		write(fds2[1], str2, sizeof(str2));
		sleep(3);
	}

	return 0;
}

