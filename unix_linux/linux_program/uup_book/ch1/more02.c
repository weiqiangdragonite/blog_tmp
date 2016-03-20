/*
 * more02.c - version 0.2 of more
 * read and print 24 lines then pause for a few spacial commands
 * feature of version 0.2: reads from /dev/tty for commands
 *
 * more usage:
 * $ more filename
 * $ command | more
 * $ more < filename
 *
 * +----> show 24 lines form input
 * | +--> print [more?] message
 * | |    Input Enter, SPACE, or q
 * | +--- if Enter, advance one line
 * +----- if SPACE
 *        if q --> exit
 */


#include <stdio.h>
#include <stdlib.h>

#define PAGELEN 24
#define LINELEN 512


void do_more(FILE *fp);
int see_more(FILE *fp);

int
main(int argc, char *argv[])
{
	FILE *fp;

	if (argc == 1) {
		do_more(stdin);
	} else {
		while (--argc) {
			if ((fp = fopen(*++argv, "r")) != NULL) {
				do_more(fp);
				fclose(fp);
			} else {
				exit(EXIT_FAILURE);
			}
		}
	}

	return 0;
}


/*
 * read PAGELEN lines, then call see_more() for further instructions
 */
void
do_more(FILE *fp)
{
	char line[LINELEN];
	int num_of_lines = 0;
	int reply;
	FILE *fp_tty;

	fp_tty = fopen("/dev/tty", "r");		/* cmd stream */
	if (fp_tty == NULL) {
		perror("fopen() /dev/tty failed:");
		exit(EXIT_FAILURE);
	}

	while (fgets(line, LINELEN, fp)) {
		if (num_of_lines == PAGELEN) {		/* full screen? */
			reply = see_more(fp_tty);	/* y: ask user */
			if (reply == 0)			/* n: done */
				break;
			num_of_lines -= reply;		/* reset count */
		}

		if (fputs(line, stdout) == EOF)		/* show line */
			exit(EXIT_FAILURE);		/* or die */

		num_of_lines++;				/* count it */
	}
}


/*
 * print message, wait for response, return # of lines to advance,
 * q means no, space means yes, CR means one line
 */
int
see_more(FILE *cmd)
{
	int c;
	printf("\033[7m more? \033[m");		/* reverse on a vt100 */
	while ((c = getc(cmd)) != EOF) {	/* read form tty */
		if (c == 'q')
			return 0;
		else if (c == ' ')
			return PAGELEN;
		else if (c == '\n')
			return 1;
	}
	return 0;
}



