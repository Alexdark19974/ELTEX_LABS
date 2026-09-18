#include "common.h"

int main(void)
{
	initscr();
	cbreak();
	noecho();

	signal(SIGWINCH, sig_handler);

	curs_set(1);

	wattron(stdscr, A_BOLD | A_BLINK);

	wmove(stdscr, 5, 15);

	wprintw(stdscr, "Hello, World!\n");

	wrefresh(stdscr);

	wgetch(stdscr);

	wattroff(stdscr, A_BOLD | A_BLINK);

	endwin();

	exit(EXIT_SUCCESS);
}

