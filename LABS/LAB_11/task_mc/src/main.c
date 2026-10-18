#include "midc.h"

int main(int argc, char **argv)
{
    struct sigaction act = {0};

    (void)argc, (void)argv;

    act.sa_sigaction = &s_handler;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGSEGV, &act, NULL);

    return start_mc();
}

void s_handler(int sig_no, siginfo_t *sig_info, void *sig_context)
{
    (void)sig_context;
    (void)sig_info;

    switch(sig_no) {
        case SIGSEGV:
            endwin();
            fprintf(stderr, "[%s:%d] programm exited with SIGSEGV\n", __func__, __LINE__);
            exit(EXIT_FAILURE);
            break;
        default:
            break;
    }
}