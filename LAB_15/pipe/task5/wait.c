#include "bash-header.h"

void wait_for_child(int child_pid)
{
    int state = 0;

    do {
        int w = waitpid(child_pid, &state, WUNTRACED | WCONTINUED);

        if (-1 == w)
        {
            handle_error_en(w, "waitpid");
        }

        if (WIFEXITED(state)) 
        {
            printf("exited, status=%d\n", WEXITSTATUS(state));

        } 
        else if (WIFSIGNALED(state)) 
        {                 
            printf("killed by signal %d\n", WTERMSIG(state));
                        
        } 
        else if (WIFSTOPPED(state)) 
        {
            printf("stopped by signal %d\n", WSTOPSIG(state));

        } 
        else if (WIFCONTINUED(state)) 
        {
            printf("continued\n");
        }
                        
    } while(!WIFEXITED(state) && !WIFSIGNALED(state));
}
