#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>
#include <curses.h>
#include <sys/ioctl.h>

void sig_winch(int signo)
{
	struct winsize size;

	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);

	resizeterm(size.ws_row, size.ws_col);
}

int main (int argc, char *argv[])
{
	WINDOW *wnd;
	WINDOW *subwnd;

	initscr();
	cbreak();
	curs_set(1);
	refresh();

	signal(SIGWINCH, sig_winch);

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
