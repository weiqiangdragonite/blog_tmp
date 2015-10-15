/*
 *
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

	if ((pid = fork()) < 0)
		return -1;
	else if (pid != 0)
		_exit(EXIT_SUCCESS);	/* parent terminates */

	/* child 1 continues */
	/* become session leader */
	if (setsid() == -1)
		return -1;

	signal(SIGHUP, SIG_IGN);

	if ((pid = fork()) < 0)
		return -1;
	else if (pid != 0)
		_exit(EXIT_SUCCESS);	/* child 1 terminates */

	/* child 2 continues */
	/* change to root directory */
	chdir("/");

	/* close all open files */
	for (i = 0; i < MAXFD; ++i)
		close(i);

	/* redirect stdin, stdout, and stderr to /dev/null */
	open("/dev/null", O_RDONLY);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);

	openlog(pname, LOG_PID, facility);

	return 0;	/* success */
} 

