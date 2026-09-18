#include "common.h"

int main(void)
{
	WINDOW *wnd;
	WINDOW *subwnd;

	initscr();
	cbreak();
	curs_set(1);
	refresh();

	signal(SIGWINCH, sig_handler);

	wnd = newwin(6, 18, 2, 4);

	box(wnd, '|', '-');

	subwnd = derwin(wnd, 4, 16, 1, 1);

	wprintw(subwnd, "Hello, brave new curses world!\n");

	wrefresh(wnd);

	delwin(subwnd);
	delwin(wnd);

	move(9, 0);

	printw("Press any key to continue...");

	refresh();

	getch();

	endwin();

	exit(EXIT_SUCCESS);
}
