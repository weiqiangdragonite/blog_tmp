/*
 * sigdemo3.c
 * purpose: show answers to signal questions
 * question1: does the handler stay in effect after a signal arrives?
 * question2: what if a signalX arrives while handling signalX?
 * question3: what if a singalX arrives while handling signalY?
 * question4: what happedns to read() when a signal arrives?
 */

#include <stdio.h>
#include <signal.h>

#define INPUTLEN	100


int
main(int argc, char *argv[])
{
	void sigint_handler(int sig);
	void sigquit_handler(int sig);
	char input[INPUTLEN];
	int nchars;

	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, sigquit_handler);

	do {
		printf("\nType a message\n");
		nchars = read(0, input, INPUTLEN - 1);
		if (nchars == -1)
			perror("read() failed");
		else {
			input[nchars] = '\0';
			printf("You typed: %s", input);
		}
	} while (strncmp(input, "quit", 4) != 0);
	
	return 0;
}

void
sigint_handler(int sig)
{
	printf("Received SIGINT signal, waiting...\n");
	sleep(5);
	printf("Leaving SIGINT handler\n");
}

void
sigquit_handler(int sig)
{
	printf("Received SIGQUIT signal, waiting...\n");
	sleep(8);
	printf("Leaving SIGQUIT handler\n");
}

