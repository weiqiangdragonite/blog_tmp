/*
 * 以TLPI为主
 */

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>


#define MAXFD 64

int
daemon_init(const char *pname, int facility)
{
	int i;
	pid_t pid;


	/* Step 1 - fork() */
	if ((pid = fork()) < 0)
		return -1;
	else if (pid != 0)
		_exit(EXIT_SUCCESS);	/* parent terminates */


	/* Step 2 - setid() */
	/* child 1 continues */
	/* become session leader */
	if (setsid() == -1)
		return -1;


	/* Step 3 - Ignore SIGHUP and fork() */
	signal(SIGHUP, SIG_IGN);

	if ((pid = fork()) < 0)
		return -1;
	else if (pid != 0)
		_exit(EXIT_SUCCESS);	/* child 1 terminates */


	/* Step 4 */
	/* child 2 continues */
	/* change to root directory */
	chdir("/");


	/* Step 5 */
	/* close all open files */
	for (i = 0; i < MAXFD; ++i)
		close(i);


	/* Step 6 */
	/* redirect stdin, stdout, and stderr to /dev/null */
	open("/dev/null", O_RDONLY);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);

	openlog(pname, LOG_PID, facility);

	return 0;	/* success */
} 

