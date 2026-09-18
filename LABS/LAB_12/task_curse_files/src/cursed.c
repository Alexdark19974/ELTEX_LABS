#include "common.h"

int main(void)
{
    initscr();
    cbreak();
    noecho();

    signal(SIGWINCH, sig_handler);

    curs_set(1);

    attron(A_BOLD /*| A_UNDERLINE | A_REVERSE*/);

    move(5, 15);

    printw("Hello, brave new curses world!\n");

    attroff(A_BOLD);

    attron(A_BLINK /*| A_DIM*/);

    move(7, 16);

    printw("Press any key to continue...");

    refresh();

    getch();

    endwin();

    exit(EXIT_SUCCESS);
}

