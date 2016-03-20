/*
 * bounce2d 1.0
 * bounce a character (default is 'O') around the screen
 * define by some parameters
 *
 * user input: s slow down x-component, S slow y-component
 *             f speed up x-component, F speed y-component
 *             Q quit
 *
 * blocks on read, but timer tick sends SIGALRM caught by ball_move
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <curses.h>
#include <sys/time.h>


#define BLK		" "
#define SYMBOL		"O"
#define TOP_ROW		5
#define BOT_ROW		20
#define LEFT_EDGE	10
#define RIGHT_EDGE	70
#define X_INIT		10	/* starting col */
#define Y_INIT		10	/* starting row */
#define TICK_PER_SEC	50	/* speed */

#define X_TTM		5
#define Y_TTM		8

struct ppball {
	int x_pos, y_pos;
	int x_ttm, y_ttm;
	int x_ttg, y_ttg;
	int x_dir, y_dir;
	char symbol;
};

struct ppball ball;

int row = 0;	/* current row */
int col = 0;	/* current column */
int dir = 0;	/* the direction we are going */


int set_ticker(int n_msecs);
void set_cr_noecho_mode(void);
void move_msg(int sig);	/* handler for timer */

int
main(int argc, char *argv[])
{
	int c;			/* user input */

	set_up();

	while ((c = getchar()) != 'Q') {
		if (c == 'f')
			ball.x_ttm--;
		else if (c == 's')
			ball.x_ttm++;
		else if (c == 'F')
			ball.y_ttm--;
		else if (c == 'S')
			ball.y_ttm++;
	}

	wrap_up();
	return 0;





	int delay;
	int new_delay;



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
tty_mode(int how)
{
	static struct termios old_mode;

	if (how == 0) {
		tcgetattr(0, &old_mode);
	} else {
		tcsetattr(0, TCSANOW, &old_mode);
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

void
set_up(void)
{
	void ball_move(int);

	ball.x_pos = X_INIT;
	ball.y_pos = Y_INIT;

	ball.x_ttg = ball.x_ttm = X_TTM;
	ball.y_ttg = ball.y_ttm = Y_TTM;

	ball.x_dir = ball.y_dir = 1;
	ball.symbol = SYMBOL;

	tty_mode(0);		/* save tty mode */
	set_cr_noecho_mode();	/* need to restore old mode! */

	initscr();
	clear();

	mvaddch(ball.y_pos, ball.x_pos, ball.symbol);
	refresh();

	signal(SIGINT, SIG_IGN);
	signal(SIGALRM, ball_move);
	set_ticker(1000 / TICK_PER_SEC);
}

void
wrap_up(void)
{
	tty_mode(1);
	set_ticker(0);
	endwin();
}




void
ball_move(int sig)
{
	int y_cur, x_cur, moved;

	signal(SIGALRM, SIG_IGN);	/* dont caught now */

	y_cur = ball.y_pos;
	x_cur = ball.x_pos;
	moved = 0;

	if (ball.y_ttm > 0 && ball.y_ttg-- == 1) {
		
	}



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








