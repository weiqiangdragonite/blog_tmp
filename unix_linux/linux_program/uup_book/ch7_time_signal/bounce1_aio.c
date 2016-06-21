/*
 * bounce_aio.c
 * purpose: animation with user control, using aio_read()
 * note: set_ticker() sends SIGALRM, handler does animation
 *       keyboard sends SIGIO, mian only calls pause()
 * compile: gcc  -lrt -lcurses
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <aio.h>
#include <curses.h>
#include <sys/time.h>


#define MSG	"hello"
#define BLK	"     "


int row = 10;		/* current row */
int col = 0;		/* current column */
int dir = 1;		/* the direction we are going */
int delay = 200;	/* how long to wait */
int done = 0;

struct aiocb kbcbuf;	/* an aio control buf */


void tty_mode(int how);
int set_ticker(int n_msecs);
void set_cr_noecho_mode(void);
void on_alarm(int sig);	/* handle for alarm */
void on_input(int sig);	/* handle for input */
void sigint_handler(int sig);
void setup_aio_buffer(void);


int
main(int argc, char *argv[])
{
	tty_mode(0);
	set_cr_noecho_mode();
	initscr();
	clear();


	signal(SIGINT, sigint_handler);
	signal(SIGIO, on_input);	/* install input handler */
	setup_aio_buffer();		/* initialize aio ctrl buff */
	aio_read(&kbcbuf);		/* place a read request */

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
	int c;
	char *cp = (char *) kbcbuf.aio_buf;

	if (aio_error(&kbcbuf) != 0) {
		perror("aio_read() failed:");
	} else {
		/* get number of chars to read */
		if (aio_return(&kbcbuf) == 1) {
			c = *cp;
			if (c == 'Q' || c == EOF)
				done = 1;
			else if (c == ' ')
				dir = -dir;
		}
	}

	/* place new request */
	aio_read(&kbcbuf);
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
setup_aio_buffer(void)
{
	static char input[1];	/* 1 char of input */

	kbcbuf.aio_fildes = 0;	/* standard input */
	kbcbuf.aio_buf = input;	/* buffer */
	kbcbuf.aio_nbytes = 1;	/* number to read */
	kbcbuf.aio_offset = 0;	/* offset in file */

	/* describe what to do when read is ready */
	kbcbuf.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	kbcbuf.aio_sigevent.sigev_signo = SIGIO;
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




