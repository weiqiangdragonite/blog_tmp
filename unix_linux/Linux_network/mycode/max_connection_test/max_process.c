/*
 * 测试能fork多少个进程
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void
child(void)
{
	long i = 0;
	/* do nothing and never return */
	while (1) {
		i = i + 1;
		sleep(1);
	}
}

int
main(int argc, char *argv[])
{
	pid_t pid;
	int counter;
	long i = 0;

	counter = 0;
	while (1) {
		if ((pid = fork()) == -1) {
			/* error */
			perror("fork() failed");
			while (1) {
				i = i + 1;
				sleep(1);
			}
		} else if (pid == 0) {
			/* child */
			child();
		}

		printf("create %d child process, pid = %d\n", ++counter, pid);
	}

	return 0;
}
