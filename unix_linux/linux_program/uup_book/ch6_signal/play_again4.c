/*
 * play_again4.c:
 * purpose: ask if user wants another transaction
 * method: set tty into char-by-char mode and no-echo mode,
 *         set tty into no-delay mode
 *         read char, return result
 *         resets terminal modes on SIGINT, ignores SIGQUIT
 * returns: 0 -> yes, 1 -> no, 2 -> timeout
 * better: reset terminal mode on Interrupt
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>


#define QUESTION	"Do you want another transaction"
#define TRIES		3		/* max tries */
#define SLEEPTIME	2		/* time per try */
#define BEEP		putchar('\a')	/* alert user */


void tty_mode(int how);
int get_ok_char(void);
void set_cr_noecho_mode(void);
void set_nodelay_mode(void);
int get_response(char *question, int maxtries);

void sigint_handler(int sig);

int
main(int argc, char *argv[])
{
	int res;

	tty_mode(0);		/* save tty mode */
	set_cr_noecho_mode();	/* set -icanon, -echo */
	set_nodelay_mode();	/* no input == EOF */

	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, SIG_IGN);

	res = get_response(QUESTION, TRIES);
	tty_mode(1);		/* restore tty mode */

	return res;
}

int
get_response(char *question, int maxtries)
{
	int input;

	printf("%s (y/n)? ", question);
	fflush(stdout);
	while (1) {
		sleep(SLEEPTIME);
		switch (input = get_ok_char()) {
		case 'y':
		case 'Y':
			putchar('\n');
			return 0;
		case 'n':
		case 'N':
			putchar('\n');
			return 1;
		}
		if (maxtries-- == 0)
			return 2;
		BEEP;
	}
}

/*
 * skip over non-legal chars and return y, Y, n, N or EOF
 */
int
get_ok_char(void)
{
	int c;
	while ((c = getchar()) != EOF && strchr("yYnN", c) == NULL)
		continue;
	return c;
}


void
set_nodelay_mode(void)
{
	int flags;
	flags = fcntl(0, F_GETFL);
	flags |= O_NDELAY;
	fcntl(0, F_SETFL, flags);
}

/*
 * how == 0 -> save current mode
 * how == 1 -> restore old mode
 */
void
tty_mode(int how)
{
	static struct termios old_mode;
	static int old_flag;

	if (how == 0) {
		tcgetattr(0, &old_mode);
		old_flag = fcntl(0, F_GETFL);
	} else {
		tcsetattr(0, TCSANOW, &old_mode);
		fcntl(0, F_SETFL, old_flag);
	}
}


/*
 *
 */
void
set_cr_noecho_mode(void)
{
	struct termios state;
	tcgetattr(0, &state);
	state.c_lflag &= ~ICANON;	/* no buffering */
	state.c_lflag &= ~ECHO;		/* no echo either */
	state.c_cc[VMIN] = 1;		/* get 1 char at a time */
	tcsetattr(0, TCSANOW, &state);	/* install settings */
}


void
sigint_handler(int sig)
{
	tty_mode(1);
	exit(1);
}
