#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

void my_pipe(char **);

int main(int argc , char *argv[])
{
    if (argc <= 4)
    {
        fprintf(stderr, "Usage: %s ls -la head -n4 (4 parameters required)\n", *argv);
        exit(EXIT_FAILURE);
    }

    my_pipe(argv + 1);

    exit(EXIT_SUCCESS);
}

void my_pipe(char **argv)
{
    pid_t c_pid_1 = 0;
    pid_t c_pid_2 = 0;
    int pipefd[2];
    int state = 0;
    int w = 0;

    state = pipe(pipefd);

    if (-1 == state)
    {
        handle_error_en(state, "pipe");
    }

    c_pid_1 = fork();

    if (0 == c_pid_1)
    {
        state = dup2(pipefd[1], STDOUT_FILENO);

        if (-1 == state) 
        {
            handle_error_en(state, "dup2");
        }

        state = close(pipefd[1]);

        if (-1 == state)
        {
            handle_error_en(state, "close");
        }

        state = close(pipefd[0]);

        if (-1 == state)
        {
            handle_error_en(state, "close");
        }

        state = execlp(argv[0], argv[0], argv[1], NULL);

        if (-1 == state)
        {
            handle_error_en(state, "execlp");
        }

    }
    else
    {
        c_pid_2 = fork();

        if (0 == c_pid_2)
        {
            state = dup2(pipefd[0], STDIN_FILENO);

            if (-1 == state) 
            {
                handle_error_en(state, "dup2");
            }

            state = close(pipefd[0]);

            if (-1 == state)
            {
                handle_error_en(state, "close");
            }

            state = close(pipefd[1]);

            if (-1 == state)
            {
                handle_error_en(state, "close");
            }

            state = execlp(argv[2], argv[2], argv[3], NULL);

            if (-1 == state)
            {
                handle_error_en(state , "execlp");
            }
        }
        else
        {
            state = close(pipefd[0]);

            if (-1 == state)
            {
                handle_error_en(state, "close");
            }

            state = close(pipefd[1]);

            if (-1 == state)
            {
                handle_error_en(state, "close");
            }


            do {
                w = waitpid(c_pid_1, &state, WUNTRACED | WCONTINUED);

                if (w == -1) 
                {
                    perror("waitpid");
                    exit(EXIT_FAILURE);
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

            } while (!WIFEXITED(state) && !WIFSIGNALED(state));

            do {
                w = waitpid(c_pid_2, &state, WUNTRACED | WCONTINUED);
                
                if (w == -1)
                {
                    perror("waitpid");
                    exit(EXIT_FAILURE);
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

            } while (!WIFEXITED(state) && !WIFSIGNALED(state));
        }
    }
}
