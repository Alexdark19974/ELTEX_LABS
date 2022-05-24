#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <curses.h>
#include <stdlib.h>

void sig_winch(int signo)
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
	resizeterm(size.ws_row, size.ws_col);
}

int main(int argc, char *argv[])
{
	initscr();
	cbreak();
	noecho();

	signal(SIGWINCH, sig_winch);

	curs_set(1);

	wattron(stdscr, A_BOLD | A_BLINK);

	wmove(stdscr, 5, 15);

	wprintw(stdscr, "Hello, World!\n");

	wrefresh(stdscr);

	wgetch(stdscr);

	wattroff(stdscr, A_BOLD | A_BLINK);

	endwin();

	return 0;
}
