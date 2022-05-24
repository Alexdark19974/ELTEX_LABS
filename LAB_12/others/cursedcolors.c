#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <curses.h>

void sig_winch(int signo)
{
	struct winsize size;

	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);

	resizeterm(size.ws_row, size.ws_col);
}

int main(int argc, char *argv[])
{
	WINDOW *wnd;
	WINDOW *subwnd;

	initscr();
	curs_set(TRUE);
	signal (SIGWINCH, sig_winch);

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
