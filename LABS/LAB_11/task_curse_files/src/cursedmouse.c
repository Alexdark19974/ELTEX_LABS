#include "common.h"

int main(void)
{
    initscr();

	signal(SIGWINCH, sig_handler);

	keypad(stdscr, 1);

	mousemask(BUTTON1_CLICKED, NULL);

	move(2, 2);

	printw("Press the left mouse button to test mouse\n");

	printw("Press any key to quit...\n");

	refresh();

	while (wgetch(stdscr) == KEY_MOUSE)
	{
		MEVENT event;

		getmouse(&event);

		move(0,0);

		printw("Mouse button pressed at %i, %i\n", event.y, event.x);

		refresh();

		move(event.y, event.x);
	}

	endwin();

	exit(EXIT_SUCCESS);
}

