/*
 * bounce_async.c
 * purpose: animation with user control, using O_ASYNC
 * note: set_ticker() sends SIGALRM, handler does animation
 *       keyboard sends SIGIO, mian only calls pause()
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <curses.h>
#include <sys/time.h>


#define MSG	"hello"
#define BLK	"     "


int row = 10;		/* current row */
int col = 0;		/* current column */
int dir = 1;		/* the direction we are going */
int delay = 200;	/* how long to wait */
int done = 0;


void tty_mode(int how);
int set_ticker(int n_msecs);
void set_cr_noecho_mode(void);
void on_alarm(int sig);	/* handle for alarm */
void on_input(int sig);	/* handle for input */
void enable_kbd_signals();
void sigint_handler(int sig);


int
main(int argc, char *argv[])
{
	tty_mode(0);
	set_cr_noecho_mode();
	initscr();
	clear();


	signal(SIGINT, sigint_handler);
	signal(SIGIO, on_input);	/* install input handler */
	enable_kbd_signals();		/* turn on kbd signals */
	signal(SIGALRM, on_alarm);	/* install alarm handler */
	set_ticker(delay);		/* start ticking */


	move(row, col);
	addstr(MSG);


	while (!done) {
		pause();
	}

	tty_mode(1);
	endwin();

	return 0;
}

void
sigint_handler(int sig)
{
	tty_mode(1);
	set_ticker(0);
	endwin();
	exit(0);
}

void
on_input(int sig)
{
	int c = getchar();

	if (c == 'Q' || c == EOF)
		done = 1;
	else if (c == ' ')
		dir = -dir;
}


void
on_alarm(int sig)
{
	signal(SIGALRM, on_alarm);	/* reset, just in case */

	//move(row, col);			/* clean */
	//addstr(BLK);
	mvaddstr(row, col, BLK);


	col += dir;
	mvaddstr(row, col, MSG);
	refresh();

	/* handle borders */
	if (dir == -1 && col <= 0)
		dir = 1;
	else if (dir == 1 && (col + strlen(MSG)) >= COLS)
		dir = -1;
}


void
enable_kbd_signals(void)
{
	int flags;

	fcntl(0, F_SETOWN, getpid());
	flags = fcntl(0, F_GETFL);
	fcntl(0, F_SETFL, (flags | O_ASYNC));
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




