
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void
handle_sigchld(int sig)
{
	int status;
	pid_t pid;

	pid = waitpid(-1, &status, WNOHANG);
	if (WIFEXITED(status)) {
		printf("Remove child PID: %ld\n", (long) pid);
		printf("Child send: %d\n", WEXITSTATUS(status));
	}
}


int
main(int argc, char *argv[])
{
	int i;
	pid_t pid;
	struct sigaction action;

	action.sa_handler = handle_sigchld;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGCHLD, &action, 0);

	pid = fork();
	if (pid < 0) {
		perror("fork() failed");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		printf("I am child process\n");
		sleep(10);
		return 12;
	} else {
		printf("Child PID: %ld\n", (long) pid);

		pid = fork();
		if (pid < 0) {
			perror("fork() failed");
			exit(EXIT_FAILURE);
		} else if (pid == 0) {
			printf("I am another child process\n");
			sleep(10);
			exit(24);
		} else {
			printf("Another Child PID: %ld\n", (long) pid);
			for (i = 0; i < 5; ++i) {
				printf("wait...\n");
				sleep(5);
			}
		}
	}

	return 0;
}




