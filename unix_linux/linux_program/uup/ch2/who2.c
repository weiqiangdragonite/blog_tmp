/*
 * who2.c - read /etc/utmp and list info therein
 *        - suppresses empty records
 *        - formats time nicely
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmp.h>
#include <fcntl.h>
#include <time.h>


#define SHOWHOST	/* include remote machine on output */


void show_info(struct utmp *utbufp);
void show_time(long timeval);


int
main(int argc, char *argv[])
{
	struct utmp utbuf;	/* read info into here */
	int utmpfd;
	int reclen = sizeof(utbuf);

	if ((utmpfd = open(UTMP_FILE, O_RDONLY)) == -1) {
		perror("open UTMP_FILE failed:");
		exit(EXIT_FAILURE);
	}

	while (read(utmpfd, &utbuf, reclen) == reclen)
		show_info(&utbuf);
	close(utmpfd);
	return 0;
}

/*
 * show info()
 * displays contents of the utmp struct in human readable form
 * * note * these size should not be hardwired
 */
void
show_info(struct utmp *utbufp)
{
	if (utbufp->ut_type != USER_PROCESS)
		return;

	printf("%-12.12s", utbufp->ut_name);	/* logname */
	printf(" ");
	printf("%-8.8s", utbufp->ut_line);	/* tty */
	printf(" ");
	show_time(utbufp->ut_time);		/* display time */
	printf(" ");
#ifdef SHOWHOST
	if (utbufp->ut_host[0] != '\0')
		printf("(%s)", utbufp->ut_host);	/* the host */
#endif
	printf("\n");
}

/*
 * display time in a format fit for human comsumption
 */
void
show_time(long timeval)
{
	char *cp;
	cp = ctime(&timeval);
	printf("%12.12s", cp+4);
}


