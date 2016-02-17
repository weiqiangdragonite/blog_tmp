/*
 * tbounce1d.c - controled animation using two threads
 * note: one thread handles animation
 *       the other handles keyboard input
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <curses.h>
#include <sys/time.h>
#include <pthread.h>


#define MSG	"hello"
#define BLK	"     "


/* shared variables both threads use. These need a mutex */
int row = 0;	/* current row */
int col = 0;	/* current column */
int dir = 0;	/* the direction we are going */
int delay;


int set_ticker(int n_msecs);
void set_cr_noecho_mode(void);

void *moving_msg();

int
main(int argc, char *argv[])
{
	int new_delay;
	int c;			/* user input */

	pthread_t msg_thread;


	set_cr_noecho_mode();	/* need to restore old mode! */
	initscr();
	clear();

	row = 10;		/* start here */
	col = 0;
	dir = 1;		/* 1-->move right, -1-->move left */
	delay = 200;		/* 200ms = 0.2s */

	move(row, col);
	addstr(MSG);
	refresh();

	pthread_create(&msg_thread, NULL, moving_msg, NULL);


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
			delay = new_delay;
	}
	pthread_cancel(msg_thread);
	endwin();
	return 0;
}

void *
moving_msg(void *arg)
{
	while (1) {
		usleep(delay * 1000);	/* sleep a while(0.2 s) */

		move(row, col);
		addstr(BLK);

		col += dir;
		move(row, col);
		addstr(MSG);
		refresh();

		if (dir == -1 && col <= 0)
			dir = 1;
		else if (dir == 1 && (col + strlen(MSG)) > COLS)
			dir = -1;
	}
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




