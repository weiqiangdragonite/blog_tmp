/*
 * 推荐参考my_daemon和APUE的daemonsize
 */



/*
 * from APUE chap 13
 */
void
daemonsize(const char *cmd)
{
	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;

	/* clear file creation mask */
	umask(0);

	/* get maximum number of file descriptors */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
		perror("getrlimit() failed");
		exit(-1);
	}

	/* become session leader to lose controlling TTY */
	if ((pid = fork()) < 0) {
		perror("fork() failed");
		exit(-1);
	} else if (pid != 0)
		exit(0);	/* parent */
	setsid();		/* child */

	/* ensure future opens won't allocate controlling TTYs */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0) {
		perror("sigaction() failed");
		exit(-1);
	}
	if ((pid = fork()) < 0) {
		perror("fork() failed");
		exit(-1);
	} else if (pid != 0)
		exit(0);	/* parent */

	/* change the current working directory to the root so
	 * we won't prevent file system from being unmounted */
	if (chdir("/") < 0) {
		perror("chdir() failed");
		exit(-1);
	}

	/* close all open file descriptors */
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; ++i)
		close(i);

	/* attach file descriptors 0, 1, and 2 to /dev/null */
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	/* initialize the log file */
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
			fd0, fd1, fd2);
		exit(-1);
	}
}


/*
 * from UNP chap 13
 */
#define MAXFD	64
int
daemon_init(const char *pname, int facility)
{
	int i;
	pid_t pid;

	if ((pid = fork()) < 0) {
		perror("fork() failed");
		return -1;
	} else if (pid != 0)
		_exit(0);	/* parent terminates */

	/* become session leader */
	if (setsid() == -1) {
		perror("setsid() failed");
		return -1;
	}

	signal(SIGHUP, SIG_IGN);

	if ((pid = fork()) < 0) {
		perror("fork() failed");
		return -1;
	} else if (pid != 0)
		_exit(0);	/* child 1 terminates */

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



/*
 * from TLPI chap 37
 */
int
become_daemon()
{
	int maxfd, fd;

	switch (fork()) {	/* become background process */
	case -1:
		return -1;
	case 0:
		break;		/* child falls through... */
	default:
		_exit(0);	/* while parent terminates */
	}

	/* become leader of new session */
	if (setsid() == -1)
		return -1;

	switch (fork()) {	/* ensure we are not session leader */
	case -1:
		return -1;
	case 0:
		break;
	default:
		_exit(0);
	}

	umask(0);	/* clear file mode creation mask */
	chdir("/");	/* change to root directory */

	/* close all open files */
	maxfd = sysconf(_SC_OPEN_MAX);
	if (maxfd == -1)
		maxfd = 1024;
	for (fd = 0; fd < maxfd; ++fd)
		close(fd);

	/* reopen standard fds to /dev/null */
	fd = open("/dev/null", O_RDWR);
	if (fd != STDIN_FILENO)
		return -1;
	if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
		return -1;
	if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
		return -1;
}




/*
 *
 */
int
my_daemon()
{
	if (fork() < 0) {
		perror("fork() failed");
		return -1;
	} else if (pid != 0)
		_exit(0);

	if (setsid() == -1) {
		perror("setsid() failed");
		return -1;
	}

	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	if (fork() < 0) {
		perror("fork() failed");
		return -1;
	} else if (pid != 0)
		_exit(0);

	umask(0);
	chdir("/");

	for (fd = 0, fdtablesize = getdtablesize(); fd < fdtablesize; ++fd)
		close(fd);

	open("/dev/null", O_RDONLY);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);
}