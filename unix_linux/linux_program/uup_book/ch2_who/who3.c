/*
 * who3.c - who with buffered reads
 *        - suppresses empty records
 *        - formats time nicely
 *        - buffers input(using utmplib)
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
	struct utmp *utbufp;	/* read info into here */

	if (utmpopen(UTMP_FILE) == -1) {
		perror("open UTMP_FILE failed:");
		exit(EXIT_FAILURE);
	}

	while ((utbufp = utmp_next()) != (struct utmp *) NULL)
		show_info(utbufp);
	utmp_close();

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


/*
 * logout_tty(char *line)
 * marks a utmp record as logged out
 * does not blank username or remote host
 * return -1 on error, 0 on success
 */
int
logout_tty(char *line)
{
	int fd;
	struct utmp rec;
	int len = sizeof(struct utmp);
	int retval = -1;	/* pessimsim */

	if ((fd = open(UTMP_FILE, O_RDWR)) == -1)
		return -1;

	/* search and replace */
	while (read(fd, &rec, len) == len) {
		if (strncmp(rec.ut_line, line, sizeof(rec.ut_line)) == 0) {
			rec.ut_type = DEAD_PROCESS;
			if (time(&rec.ut_time) == -1)
				break;
			if (lseek(fd, -len, SEEK_CUR) == -1)
				break;
			if (write(fd, &rec, len) == len)
				retval = 0;
			break;
		}
	}
	if (close(fd) == -1)
		retval = -1;
	return retval;
}





