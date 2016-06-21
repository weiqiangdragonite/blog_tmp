/*
 * who1.c - a first version of the who program
 *          open, read UTMP file, and show results
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utmp.h>
#include <fcntl.h>


#define SHOWHOST	/* include remote machine on output */


void show_info(struct utmp *utbufp);

int
main(int argc, char *argv[])
{
	struct utmp current_record;	/* read info into here */
	int utmpfd;
	int reclen = sizeof(current_record);

	/* UTMP_FILE is in utmp.h */
	if ((utmpfd = open(UTMP_FILE, O_RDONLY)) == -1) {
		perror("open UTMP_FILE failed:");
		exit(EXIT_FAILURE);
	}

	while (read(utmpfd, &current_record, reclen) == reclen)
		show_info(&current_record);
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
	printf("%-8.8s", utbufp->ut_name);	/* logname */
	printf(" ");
	printf("%-8.8s", utbufp->ut_line);	/* tty */
	printf(" ");
	printf("%10ld", utbufp->ut_time);	/* login time */
	printf(" ");
#ifdef SHOWHOST
	printf("(%s)", utbufp->ut_host);	/* the host */
#endif
	printf("\n");
}


