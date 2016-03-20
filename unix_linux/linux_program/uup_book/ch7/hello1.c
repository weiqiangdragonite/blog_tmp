/*
 * hello1.c
 * purpose - show the minimal calls needed to use curses
 * outline - initialize, draw stuff, wait for input, quit
 * compile: gcc hello1.c -lcurses
 */


#include <stdio.h>
#include <curses.h>

int
main(int argc, char *argv[])
{
	initscr();	/* turn on curses */
	clear();

	move(10, 20);
	addstr("Helo, world");
	move(LINES - 1, 0);
	refresh();	/* update screen */
	getch();	/* wait for user input */
	endwin();	/* turn off curses */

	return 0;
}


