/*
 * hello2.c
 * purpose - show to use curses functions with a loop
 * outline - initialize, draw stuff, wrap up
 */


#include <stdio.h>
#include <curses.h>

int
main(int argc, char *argv[])
{
	int i;

	initscr();	/* turn on curses */
	clear();

	for (i = 0; i < LINES; ++i) {
		move(i, i + i);
		if (i % 2 == 1)
			standout();
		addstr("Hello, world");
		if (i % 2 == 1)
			standend();
	}
	refresh();	/* update screen */
	getch();	/* wait for user input */
	endwin();	/* turn off curses */

	return 0;
}


