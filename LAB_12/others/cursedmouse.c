#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <curses.h>
#include <signal.h>

void sig_winch(int);

int main(int argc, char *argv[])
{
    initscr();

	signal(SIGWINCH, sig_winch);

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

void sig_winch(int signo)
{
	struct winsize size;

	ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);

	resizeterm(size.ws_row, size.ws_col);

	nodelay(stdscr, 1);

	while (wgetch(stdscr) != ERR)
    {
        ;
    }

    nodelay(stdscr, 0);
}
