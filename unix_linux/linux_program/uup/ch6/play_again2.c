/*
 * play_again2.c:
 * purpose: ask if user wants another transaction
 * method: set tty into char-by-char mode and no-echo mode,
 *         read char, return result
 * returns: 0 -> yes, 1 -> no
 * better: timeout if user walks away
 */


#include <stdio.h>
#include <termios.h>

#define QUESTION	"Do you want another transaction"

void tty_mode(int how);
void set_cr_noecho_mode(void);
int get_response(char *question);

int
main(int argc, char *argv[])
{
	int res;

	tty_mode(0);		/* save tty mode */
	set_cr_noecho_mode();	/* set -icanon, -echo */
	res = get_response(QUESTION);
	tty_mode(1);		/* restore tty mode */

	return res;
}

int
get_response(char *question)
{
	int input;

	printf("%s (y/n)? ", question);
	while (1) {
		switch (input = getchar()) {
		case 'y':
		case 'Y':
			putchar('\n');
			return 0;
		case 'n':
		case 'N':
		case EOF:
			putchar('\n');
			return 1;
		}
	}
}


/*
 * how == 0 -> save current mode
 * how == 1 -> restore old mode
 */
void
tty_mode(int how)
{
	static struct termios old_mode;
	if (how == 0)
		tcgetattr(0, &old_mode);
	else
		tcsetattr(0, TCSANOW, &old_mode);
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
