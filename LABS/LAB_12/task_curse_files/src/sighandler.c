#include "common.h"

void sig_handler(int signum)
{
    struct winsize size;
    
    printf("signum=%d\n", signum);

    switch(signum) {
        case SIGWINCH:
            ioctl(fileno(stdout), TIOCGWINSZ , (char *) &size);
            resizeterm(size.ws_row, size.ws_col);
            break;
        default:
            break;
    }
}
