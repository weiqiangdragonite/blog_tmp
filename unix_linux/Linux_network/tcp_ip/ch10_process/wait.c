
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/wait.h>

int
main(int argc, char *argv[])
{
	int status;
	pid_t pid;

	pid = fork();
	if (pid < 0) {
		perror("fork() failed");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		return 3;
	} else {
		printf("Child PID: %ld\n", (long) pid);

		pid = fork();
		if (pid < 0) {
			perror("fork() failed");
			exit(EXIT_FAILURE);
		} else if (pid == 0) {
			exit(7);
		} else {
			printf("Child PID: %ld\n", (long) pid);

			wait(&status);
			if (WIFEXITED(status))
				printf("Child send: %d\n", WEXITSTATUS(status));

			wait(&status);
			if (WIFEXITED(status))
				printf("Child send: %d\n", WEXITSTATUS(status));

			sleep(30);
		}
	}

	return 0;
}

