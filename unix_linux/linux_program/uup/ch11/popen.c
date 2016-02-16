/*
 * popen.c - a version of the UNIX popen() library function
 * todo: what about signal handling for child process?
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


#define READ	0
#define WRITE	1


FILE *
popen(const char *cmd, const char *mode)
{
	int pfp[2];
	pid_t pid;
	FILE *fp;
	int parent_end, child_end;

	if (*mode == 'r') {
		parent_end = READ;
		child_end = WRITE;
	} else if (*mode == 'w') {
		parent_end = WRITE;
		child_end = READ;
	} else
		return NULL;

	if (pipe(pfp) == -1)
		return NULL;

	if ((pid = fork()) == -1) {
		close(pfp[0]);
		close(pfp[1]);
		return NULL;
	}

	/* parent */
	if (pid > 0) {
		if (close(pfp[child_end]) == -1)
			return NULL;
		/* return same mode */
		return fdopen(pfp[parent_end], mode);
	}


	/* child */
	if (close(pfp[parent_end]) == -1)
		exit(1);

	if (dup2(pfp[child_end], child_end) == -1)
		exit(1);

	if (close(pfp[child_end]) == -1)
		exit(1);

	execl("/bin/sh", "sh", "-c", cmd, NULL);
	exit(1);
}

