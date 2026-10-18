#include "nano.h"

extern nano_s nano;

void s_handler(int sig_no, siginfo_t *sig_info, void *sig_context)
{
    struct winsize size;

    (void)sig_no;
    (void)sig_info;
    (void)sig_context;
    // printf("signo=%d\n", sig_no);

    switch(sig_no) {
        case SIGWINCH:
            ioctl(STDOUT_FILENO, TIOCGWINSZ, (char *)&size);
            resizeterm(size.ws_row, size.ws_col);
/*
            clear();

            nano.redraw(&nano, STDSCR);

            mvprintw(0, 3, DISPLAY_EDITOR_NAME);
            mvprintw(0, nano.m_x / 2, DISPLAY_NEW_BUFFER);    
            attroff(A_REVERSE);

            nano.redraw(&nano, MENUSCR);

            nano.c_y = nano.c_x = 1;
            move(nano.c_y, nano.c_x);
            curs_set(TRUE);
*/
            break;
        default:
            break;
    }
}
