/*
 * play_again1.c:
 * purpose: ask if user wants another transaction
 * method: set tty into char-by-char mode, read char, return result
 * returns: 0 -> yes, 1 -> no
 * better: do no echo inappropriate input
 */


#include <stdio.h>
#include <termios.h>

#define QUESTION	"Do you want another transaction"

void tty_mode(int how);
void set_crmode(void);
int get_response(char *question);

int
main(int argc, char *argv[])
{
	int res;

	tty_mode(0);	/* save tty mode */
	set_crmode();	/* set char-by-char mode */
	res = get_response(QUESTION);
	tty_mode(1);	/* restore tty mode */

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
		default:
			printf("\nCannot understand %c, ", input);
			printf("please type y or n\n");
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
set_crmode(void)
{
	struct termios state;
	tcgetattr(0, &state);
	state.c_lflag &= ~ICANON;	/* no buffering */
	state.c_cc[VMIN] = 1;		/* get 1 char at a time */
	tcsetattr(0, TCSANOW, &state);	/* install settings */
}
