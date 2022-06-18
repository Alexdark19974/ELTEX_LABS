#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#define ARR_SIZE 100
#define MSG_SIZE 30
#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)


void piped_exchange(char **);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s Hello!\n", *argv);
        exit(EXIT_FAILURE);
    }

    piped_exchange(argv + 1);

    exit(EXIT_SUCCESS);
}

void piped_exchange(char **argv)
{
    int pipefd[2];
    int w = 0;
    int state = 0;
    int c_pid = 0;
    char buf[ARR_SIZE] = {0};
    char *arg = strdup(*argv);

    state = pipe(pipefd);
    
    if (-1 == state) 
    {
        handle_error_en(state, "pipe");
    }

    printf("sending %s from parent process: %d\n", *argv, getppid());
    /*fflush(stdout);*/                                                           // no need for fflush(), since printf has '\n' character.
    sync();                                                                       // syncing  with the kernel buffer to make sure the message is on screen before fork.

    state = write(pipefd[1], argv[0], strlen(arg));

    if (-1 == state) 
    {
        handle_error_en(state, "write");
    }

    c_pid = fork();

    if (-1 == c_pid) 
    {
        handle_error_en(c_pid, "fork");
    }

    if (0 == c_pid)
    {
        char child_message[MSG_SIZE] = {"sent love from child"};

        state = read(pipefd[0], buf, strlen(buf));

        if (-1 == state) 
        {
            handle_error_en(state, "read");
        }

        close(pipefd[0]);

        if (-1 == state) 
        {
            handle_error_en(state, "close");
        }

        printf("child process: %d got the message %s\n", getpid(), buf);    // step 2: show that the child got the message from the parent
        /*fflush(stdout);*/                                                 // no need for fflush(), since printf has '\n' character.
        sync();                                                             // syncing  with the kernel buffer to make sure the message before termination.

        for (int i = 0; buf[i] != '\0'; i++)
        {
            buf[i] = '\0';
        }

        char *tmp = strncpy(buf, child_message, strlen(buf - 1));

        if (tmp != buf)
        {
            handle_error("strncpy");
        }

        buf[strlen(buf) - 1] = '\0';

        state = write(pipefd[1], buf, strlen(buf));

        if (-1 == state) 
        {
            handle_error_en(state, "write");
        }

        state = close(pipefd[1]);

        if (-1 == state) 
        {
            handle_error_en(state, "close");
        }

        exit(EXIT_SUCCESS);
    }
    else
    {
        do {
            w = waitpid(c_pid, &state, WUNTRACED | WCONTINUED);

            if (w == -1) 
            {
                handle_error_en(state, "waitpid");
            }

            /*if (WIFEXITED(state))                                         // commented out since if the exit status is succesful, no need to notify the user of it.
            {
                printf("exited, status=%d\n", WEXITSTATUS(state));   
            }*/

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

        for (int i = 0; buf[i] != '\0'; i++)
        {
            buf[i] = '\0';
        }

        state = read(pipefd[0], buf, MSG_SIZE);

        if (-1 == state) 
        {
            handle_error_en(state, "read");
        }

        state = close(pipefd[0]);

        if (-1 == state) 
        {
            handle_error_en(state, "close");
        }

        state = close(STDIN_FILENO);

        if (-1 == state) 
        {
            handle_error_en(state, "close");
        }

        printf("here's proof that the parent process got the message from the child thru the read end of the pipe: ");
        fflush(stdout);
        sync();

        state = write(STDOUT_FILENO, buf, strlen(buf));

        if (-1 == state) 
        {
            handle_error_en(state, "close");
        }
        state = write(STDOUT_FILENO, "\n", sizeof(char));

        if (-1 == state) 
        {
            handle_error_en(state, "close");
        }

        state = close(STDOUT_FILENO);

        if (-1 == state) 
        {
            handle_error_en(state, "close");
        }

        free(arg);
        arg = NULL;
    }
}
