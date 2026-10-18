#include "common.h"

int main(void)
{
	WINDOW *wnd;
	WINDOW *subwnd;
	
    signal(SIGWINCH, sig_handler);

	initscr();
	curs_set(TRUE);

	start_color();

	wbkgd(stdscr, COLOR_PAIR(0));

	refresh();

	wnd = newwin(5, 18, 2, 4);

	init_pair(1, COLOR_BLUE, COLOR_GREEN);

	wattron(wnd, COLOR_PAIR(1) | A_BOLD);

	box(wnd, '|', '-');

	subwnd = derwin(wnd, 3, 16, 1, 1);

	init_pair(2, COLOR_RED, COLOR_BLUE);

	wbkgd(subwnd, COLOR_PAIR(2));

	wattron(subwnd, A_BOLD);

	wprintw(subwnd, "Hello, brave new curses world!\n");

	wrefresh(subwnd);

	wrefresh(wnd);

	delwin(subwnd);

	delwin(wnd);

	wmove(stdscr, 8, 1);

	printw("Press any key to continue...");

	refresh();

	getch();

	endwin();

	exit(EXIT_SUCCESS);
}

