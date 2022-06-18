#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

void double_pipe(char **);

int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        fprintf(stderr, "Usage: %s ls -la | tail -n4 | nl\n", *argv);
    }

    double_pipe(argv + 1);

    exit(EXIT_SUCCESS);
}

void double_pipe(char **argv)
{
    int pipefd1[2];
    int pipefd2[2];
    int c_pid_1 = 0;
    int c_pid_2 = 0;
    int c_pid_3 = 0;
    int state = 0;
    int w = 0;

    state = pipe(pipefd1);

    if (-1 == state)
    {
        handle_error_en(state, "pipe");
    }

    c_pid_1 = fork();

    if (-1 == c_pid_1)
    {
        handle_error_en(c_pid_1, "fork");
    }

    if (0 == c_pid_1)
    {
        state = dup2(pipefd1[1], STDOUT_FILENO);

        if (-1 == state) 
        {
            handle_error_en(state, "dup2");
        }

        state = close(pipefd1[1]);

        if (-1 == state)
        {
            handle_error_en(state, "close");
        }

        state = close(pipefd1[0]);

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

        state = pipe(pipefd2);

        if (-1 == state)
        {
            handle_error_en(state, "pipe");
        }

        c_pid_2 = fork();

        if (-1 == c_pid_2)
        {
            handle_error_en(state, "fork");
        }

        if (0 == c_pid_2)
        {
            dup2(pipefd1[0], STDIN_FILENO);

            if (-1 == state) 
            {
                handle_error_en(state, "dup2");
            }

            dup2(pipefd2[1], STDOUT_FILENO);

            if (-1 == state) 
            {
                handle_error_en(state, "dup2");
            }

            close(pipefd1[0]);

            if (-1 == state) 
            {
                handle_error_en(state, "close");
            }

            close(pipefd1[1]);

            if (-1 == state) 
            {
                handle_error_en(state, "close");
            }

            close(pipefd2[0]);

            if (-1 == state) 
            {
                handle_error_en(state, "close");
            }

            close(pipefd2[1]);

            if (-1 == state) 
            {
                handle_error_en(state, "close");
            }

            execlp(argv[2], argv[2], argv[3], NULL);
        }
        else
        {
            c_pid_3 = fork();

            if (-1 == c_pid_3) 
            {
                handle_error_en(state, "fork");
            }

            if (0 == c_pid_3)
            {
                state = dup2(pipefd2[0], STDIN_FILENO);

                if (-1 == state) 
                {
                    handle_error_en(state, "dup2");
                }

                state = close(pipefd2[0]);

                if (-1 == state) 
                {
                    handle_error_en(state, "close");
                }

                close(pipefd2[1]);

                if (-1 == state) 
                {
                    handle_error_en(state, "close");
                }

                close(pipefd1[0]);

                if (-1 == state) 
                {
                    handle_error_en(state, "close");
                }

                close(pipefd1[1]);

                if (-1 == state) 
                {
                    handle_error_en(state, "close");
                }

                execlp(argv[4], argv[4], argv[5], NULL);

                if (-1 == state) 
                {
                    handle_error_en(state, "execlp");
                }                
            }
            else
            {
                close(pipefd1[0]);

                if (-1 == state) 
                {
                    handle_error_en(state, "close");
                }

                close(pipefd1[1]);

                if (-1 == state) 
                {
                    handle_error_en(state, "close");
                }

                close(pipefd2[0]);

                if (-1 == state) 
                {
                    handle_error_en(state, "close");
                }

                close(pipefd2[1]);

                if (-1 == state) 
                {
                    handle_error_en(state, "close");
                }

                do {
                   w = waitpid(c_pid_1, &state, WUNTRACED | WCONTINUED);

                   if (w == -1) 
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

                   } else if (WIFCONTINUED(state)) 
                   {
                       printf("continued\n");
                   }

               } while (!WIFEXITED(state) && !WIFSIGNALED(state));

                do {
                   w = waitpid(c_pid_2, &state, WUNTRACED | WCONTINUED);

                   if (w == -1) 
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

               } while (!WIFEXITED(state) && !WIFSIGNALED(state));

                do {
                   w = waitpid(c_pid_3, &state, WUNTRACED | WCONTINUED);
                   if (w == -1) 
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
                   else if (WIFSTOPPED(state)) {
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
}
