/*
 * tanimate.c - animate serveral strings using threads, curses
 *
 * bigidea: one thread for each animated string
 *          one thread for keyboard control
 *          shared variabled for communication
 * todo: needs locks for shared variables
 *       nice to put screen handling in its own thread
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

#define MAXMSG		10	/* limit to number of strings */
#define TUNIT		20000	/* timeunits in microseconds */


struct propset {
	char *str;		/* the message */
	int row;		/* the row */
	int delay;		/* delay in time units */
	int dir;		/* direction */
};

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


int setup(int nstrs, char *strings[], struct propset props[]);
void set_cr_noecho_mode(void);
void *animate(void *arg);

int
main(int argc, char *argv[])
{
	int c, i;
	pthread_t	threads[MAXMSG];
	struct propset	props[MAXMSG];
	int num_msg;	/* number of strings */


	if (argc == 1) {
		fprintf(stderr, "Usage: %s <string> ...\n", argv[0]);
		exit(1);
	}

	num_msg = setup(argc - 1, argv + 1, props);

	/* create all threads */
	for (i = 0; i < num_msg; ++i)
		pthread_create(&threads[i], NULL, animate, &props[i]);

	/* process user input */
	while (1) {
		c = getchar();
		if (c == 'Q')
			break;
		if (c == ' ') {
			for (i = 0; i < num_msg; ++i)
				props[i].dir = -props[i].dir;
		}
		if (c >= '0' && c <= '9') {
			i = c - '0';
			if (i < num_msg)
				props[i].dir = -props[i].dir;
		}
	}

	/* cancel all the threads */
	pthread_mutex_lock(&lock);
	for (i = 0; i < num_msg; ++i)
		pthread_cancel(threads[i]);
	pthread_mutex_unlock(&lock);
	endwin();

	return 0;
}



int
setup(int nstrs, char *strings[], struct propset props[])
{
	int num_msg = (nstrs > MAXMSG ? MAXMSG : nstrs);
	int i;

	/* assign rows and velocities to each string */
	srand(getpid());
	for (i = 0; i < num_msg; ++i) {
		props[i].str = strings[i];
		props[i].row = i;
		props[i].delay = 1 + (rand() % 15);
		props[i].dir = ((rand() % 2) ? 1 : -1);
	}

	/* setup curses */
	set_cr_noecho_mode();	/* need to restore old mode! */
	initscr();
	clear();

	mvprintw(LINES-1, 0, "'Q' to quit, '0'...'%d' to bounce", num_msg-1);

	return num_msg;
}

void *
animate(void *arg)
{
	struct propset *info = (struct propset *) arg;
	int len = strlen(info->str) + 2;
	int col = rand() % (COLS - len - 3);

	while (1) {
		usleep(info->delay * TUNIT);

		pthread_mutex_lock(&lock);
			move(info->row, col);
			addch(' ');
			addstr(info->str);
			addch(' ');

			move(LINES-1, COLS-1);
			refresh();
		pthread_mutex_unlock(&lock);

		col += info->dir;

		if (info->dir == -1 && col <= 0)
			info->dir = 1;
		else if (info->dir == 1 && (col + len) > COLS)
			info->dir = -1;
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



