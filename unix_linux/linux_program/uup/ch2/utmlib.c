/*
 * utmplib.c - functions to buffer reads from utmp file
 *
 * functions are
 *     utmp_open(filename) - open file
 *         returns -1 on error
 *     utmp_next() - return pointer to next struct
 *         returns NULL on eof
 *     utmp_close() - close file
 *
 * reads NCRES per read and then doles them out from the buffer
 *
 */


#include <stdio.h>
#include <fcntl.h>
#include <utmp.h>

#define NCRES	16
#define UTSIZE	(sizeof(struct utmp))


static char utmpbuf[NCRES * UTSIZE];	/* storage */
static int num_recs;			/* num stored */
static int cur_rec;			/* next to go */
static int fd_utmp = -1;


int
utmp_open(char *filename)
{
	fd_utmp = open(filename, O_RDONLY);
	cur_rec = num_recs = 0;
	return fd_utmp;
}

struct utmp *
utmp_next(void)
{
	struct utmp *recp;

	if (fd_utmp == -1)	/* error */
		return (struct utmp *) NULL;

	if (cur_rec == num_recs && utmp_reload() == 0)
		return (struct utmp *) NULL;

	recp = (struct utmp *) &utmpbuf[cur_rec * UTSIZE];
	++cur_rec;

	return recp;
}


/*
 * read next bunch of records into buffer
 */
int
utmp_reload(void)
{
	int amt_read;	/* read them in */

	/* how many did we get */
	amt_read = read(fd_utmp, utmpbuf, NRECS * UTSIZE);
	if (amt_read == -1) {
		perror("utmp_reload() failed");
		return 0;
	}

	/* reset pointer */
	num_recs = amt_read / UTSIZE;

	cur_rec = 0;

	return num_recs;
}


void
utmp_close(void)
{
	if (fd_utmp != -1)
		close(fd_utmp);
}

