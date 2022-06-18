#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
        do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define ARR_SIZE 100
void proc_exchange(char **);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        fprintf(stderr, "Usage: %s %s\n", *argv, "Hello!");
        exit(EXIT_FAILURE);
    }

    proc_exchange(argv + 1);

    exit(EXIT_SUCCESS);
}

void proc_exchange(char **argv)
{
    int pipefd[2];
    int state = 0;
    pid_t c_pid = 0;
    char buf[ARR_SIZE] = {0};

    state = pipe(pipefd);

    if (0 != state)
    {
        handle_error_en(state, "pipe");  
    }

    c_pid = fork();

    if (-1 == c_pid)
    {
        handle_error_en(c_pid, "fork");
    }

    if (0 == c_pid)
    {
        state = close(pipefd[1]); //closing the write part

        if (-1 == state)
        {
            handle_error_en(state, "close");
        }

        printf("child process %d received ", getpid());

        fflush(stdout); // to make sure the message is printed on screen in the right order
        sync(); // pushing down to the kernel buffer

        state = read(pipefd[0], buf, ARR_SIZE); // getting data from parent process

        if (-1 == state)
        {
            handle_error_en(state, "read");
        }

        state = close(pipefd[0]); // closing the read part. not needed anymore

        if (-1 == state)
        {
            handle_error_en(state, "close");
        }

        state = write(STDOUT_FILENO, buf, strlen(buf)); //getting the data in buf out on screen

        if (-1 == state)
        {
            handle_error_en(state, "write");
        }

        state = write(STDOUT_FILENO, "\n", sizeof(char)); //newline character for readability 

        if (-1 == state)
        {
            handle_error_en(state, "write");
        }

        exit(EXIT_SUCCESS);  // success
    }
    else
    {   
        state = close(pipefd[0]); //closing the read part

        if (0 != state)
        {
            handle_error_en(state, "close");
        }

        printf("Inserted %s from parent process %d\n", *argv, getppid());

        sync(); //to make sure message appereas on screen first

        state = write(pipefd[1], *argv, strlen(*argv));
        
        if (-1 == state)
        {
            handle_error_en(state, "write");
        }

        state = close(pipefd[1]); //write part not needed anymore

        if (-1 == state)
        {
            handle_error_en(state, "close");
        }

        state = wait(&c_pid); //parent process awaits status from child process

        if (-1 == state)
        {
            handle_error_en(state, "wait");
        }
    }
}
