/*
 * bounce1d.c
 * purpose: animation with user controlled speed and direction
 * note: the handler does the animation
 *       the main program read keyboard input
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <curses.h>
#include <sys/time.h>


#define MSG	"hello"
#define BLK	"     "


int row = 0;	/* current row */
int col = 0;	/* current column */
int dir = 0;	/* the direction we are going */


int set_ticker(int n_msecs);
void set_cr_noecho_mode(void);
void move_msg(int sig);	/* handler for timer */

int
main(int argc, char *argv[])
{
	int delay;
	int new_delay;
	int c;			/* user input */



	set_cr_noecho_mode();	/* need to restore old mode! */
	initscr();
	clear();

	row = 10;		/* start here */
	col = 0;
	dir = 1;		/* 1-->move right, -1-->move left */
	delay = 200;		/* 200ms = 0.2s */

	move(row, col);
	addstr(MSG);

	signal(SIGALRM, move_msg);
	set_ticker(delay);

	while (1) {
		new_delay = 0;
		c = getchar();
		if (c == 'Q')
			break;
		if (c == ' ')
			dir = -dir;
		if (c == 'f' && delay > 2)
			new_delay = delay / 2;
		if (c == 's')
			new_delay = delay * 2;
		if (new_delay > 0)
			set_ticker(delay = new_delay);
	}

	endwin();
	return 0;
}

void
move_msg(int sig)
{
	signal(SIGALRM, move_msg);	/* reset, just in case */

	move(row, col);			/* clean */
	addstr(BLK);

	col += dir;
	move(row, col);
	addstr(MSG);
	refresh();

	if (dir == -1 && col <= 0)
		dir = 1;
	else if (dir == 1 && (col + strlen(MSG)) >= COLS)
		dir = -1;
}


int
set_ticker(int n_msecs)
{
	struct itimerval timer;
	long n_sec, n_usecs;

	n_sec = n_msecs / 1000;
	n_usecs = (n_msecs % 1000) * 1000L;

	timer.it_interval.tv_sec = n_sec;
	timer.it_interval.tv_usec = n_usecs;

	timer.it_value.tv_sec = n_sec;
	timer.it_value.tv_usec = n_usecs;

	return setitimer(ITIMER_REAL, &timer, NULL);
}


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




