#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>

void sig_winch(int signo)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ , (char *) &size);
    resizeterm(size.ws_row, size.ws_col);
}

int main(int argc, char *argv[])
{
    initscr();
    cbreak();
    noecho();

    signal(SIGWINCH, sig_winch);

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

fdsfdsO