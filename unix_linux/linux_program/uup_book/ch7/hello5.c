/*
 * hello5.c
 * purpose - bounce a message back and forth across the screen
 */


#include <stdio.h>
#include <curses.h>

#define LEFTEDGE	10
#define RIGHTEDGE	30
#define ROW		10

int
main(int argc, char *argv[])
{
	char *msg = "Hello";
	char *blk = "     ";
	int dir = 1;
	int pos = LEFTEDGE;

	initscr();	/* turn on curses */
	clear();

	while (1) {
		move(ROW, pos);
		addstr(msg);

		move(LINES - 1, COLS - 1);	/* park the cursor */
		refresh();
		sleep(1);

		move(ROW, pos);		/* erase string */
		addstr(blk);

		pos += dir;		/*advance position */
		if (pos >= RIGHTEDGE)	/* check for bounce */
			dir -= 1;
		if (pos <= LEFTEDGE)
			dir += 1;
		
	}
	endwin();	/* turn off curses */

	return 0;
}


