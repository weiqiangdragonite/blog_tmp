/*
 * tinybc.c
 *
 * A tiny calculator that uses dc to do its work
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void be_dc(int in[2], int out[2]);
void be_bc(int todc[2], int fromdc[2]);


int
main(int argc, char *argv[])
{
	int todc[2], fromdc[2];
	pid_t pid;

	if (pipe(todc) == -1 || pipe(fromdc) == -1) {
		perror("pipe() failed");
		exit(1);
	}

	if ((pid = fork()) == -1) {
		perror("fork() failed");
		exit(1);
	}

	if (pid == 0) {
		be_dc(todc, fromdc);	/* child is dc */
	} else {
		be_bc(todc, fromdc);	/* parent is bc */
		wait(NULL);
	}
}


void
be_dc(int in[2], int out[2])
{
	/* copy read to 0 */
	if (dup2(in[0], 0) == -1) {
		perror("dc: cannot redirect stdin");
		exit(1);
	}
	close(in[0]);
	close(in[1]);

	/* copy write to 1 */
	if (dup2(out[1], 1) == -1) {
		perror("dc: cannot redirect stdout");
		exit(1);
	}

	/* exec dc */
	execlp("dc", "dc", "-", NULL);
	perror("dc: execlp() failed");
	exit(1);
}


void
be_bc(int todc[2], int fromdc[2])
{
#define BUFSIZE		1024

	int num1, num2;
	char op;
	char msg[BUFSIZE];
	FILE *fpout, *fpin;

	close(todc[0]);		/* won't read from pipe to dc */
	close(fromdc[1]);	/* won't write to pipe from dc */


	fpout = fdopen(todc[1], "w");
	fpin = fdopen(fromdc[0], "r");
	if (fpout == NULL || fpin == NULL) {
		perror("fdopen() failed");
		exit(1);
	}

	while (1) {
		printf("tinybc: ");
		if (fgets(msg, BUFSIZE, stdin) == NULL)
			break;

		/* parse input */
		if (sscanf(msg, "%d %[-+*/^] %d", &num1, &op, &num2) != 3) {
			fprintf(stderr, "syntax error\n");
			continue;
		}

		if (fprintf(fpout, "%d\n%d\n%c\np\n", num1, num2, op) == EOF) {
			fprintf(stderr, "fprintf() failed");
			exit(1);
		}
		fflush(fpout);

		if (fgets(msg, BUFSIZE, fpin) == NULL)
			break;
		printf("%d %c %d = %s", num1, op, num2, msg);
	}
	fclose(fpout);
	fclose(fpout);
}


