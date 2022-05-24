#include "nano_h.h"

int main(int argc, char *argv[])
{
	initscr();
	cbreak();
	curs_set(TRUE);

	signal(SIGWINCH, sig_winch);

	refresh();

	input(argv);

	endwin();

	exit(EXIT_SUCCESS);
}
