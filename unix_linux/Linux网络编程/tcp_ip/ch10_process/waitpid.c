
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
		sleep(15);
		return 3;
	} else {
		while (!waitpid(-1, &status, WNOHANG)) {
			sleep(3);
			printf("sleep 3sec.\n");
		}

		if (WIFEXITED(status))
			printf("Child send: %d\n", WEXITSTATUS(status));
	}

	return 0;
}

