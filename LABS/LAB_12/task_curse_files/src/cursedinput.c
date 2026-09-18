#include "common.h"

int main(void)
{
	WINDOW *wnd = NULL;

	char name[MAX_NAME_LEN + 1] = {0};

	initscr();

	signal(SIGWINCH, sig_handler);

	curs_set(TRUE);

	start_color();

	refresh();

	init_pair(1, COLOR_YELLOW, COLOR_WHITE);

	wnd = newwin(5, 23, 2, 2);

	//wattron(wnd, COLOR_PAIR(1));

	//box(wnd, '|', '-');

	wbkgd(wnd, COLOR_PAIR(1));

	wattron(wnd, A_BOLD);

	wprintw(wnd, "Enter your name...\n");

	wgetnstr(wnd, name, MAX_NAME_LEN);

	wprintw(wnd, "Hello, %s!", name);

	wrefresh(wnd);

	delwin(wnd);

	curs_set(FALSE);

	move(8, 4);

	printw("Press any key to continue...");

	refresh();

//	nocbreak();

	getch();

	endwin();

	exit(EXIT_SUCCESS);
}

