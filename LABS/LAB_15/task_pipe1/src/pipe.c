#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#define READ    0
#define WRITE   1
#define BUF_SIZ 256
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)
#define handle_error_msg(fmt, ...) do { \
                            fprintf(stderr, fmt, __VA_ARGS__); \
                        } while(0)
#define ARR_SIZE 100
int proc_exchange(char **argv);

int main(int argc, char **argv)
{
    if (argc == 1) {
        handle_error_msg("[%s:%d] Usage %s %s\n", __func__, __LINE__, argv[0], "Hello");
        return EXIT_FAILURE;
    }

    return proc_exchange(argv + 1);
}

int proc_exchange(char **argv)
{
    int pipefd[2]; // pipefd[0] is the read end, pipefd[1] is the write end
    int ret = 0;
    pid_t c_pid = 0;
    char buf[ARR_SIZE] = {0};

    ret = pipe(pipefd); // pipe() return array of 2 file descriptors
    if (ret < 0) {
        handle_error("pipe", errno);
        ret = EXIT_FAILURE;
        goto err;
    }

    printf("read fd=%d, write fd=%d\n", pipefd[0], pipefd[1]);

    c_pid = fork();
    if (c_pid < 0) {
        handle_error("fork", errno);
        ret = EXIT_FAILURE;
        goto err;
    }

    /* child section */
    /* The  child  inherits  copies  of the parent's set of open file descriptors. Each file descriptor in the child refers to the same open
        file description (see open(2)) as the corresponding file descriptor in the parent. This means that the two file descriptors share
        open file status flags, file offset, and signal-driven I/O attributes (see the description of F_SETOWN and F_SETSIG in fcntl(2)). */
    if (0 == c_pid) {
        c_pid = getpid();
        printf("[chid:%d] closed the write end of pipe\n", c_pid);
        ret = close(pipefd[WRITE]); // closing the write part, child only reads the output
        if (ret < 0) {
            handle_error("close", errno);
            ret = EXIT_FAILURE;
            goto err;
        }

        ret = read(pipefd[READ], buf, ARR_SIZE); // getting data from parent process
        if (ret < 0) {
            handle_error("read", errno);
            ret = EXIT_FAILURE;
            goto err;
        }

        ret = close(pipefd[READ]); // closing the read part. Not needed anymore
        if (ret < 0) {
            handle_error("close", errno);
            ret = EXIT_FAILURE;
            goto err;
        }

        buf[strlen(buf)] = '\n';

        ret = write(STDOUT_FILENO, buf, strlen(buf)); // writing raw data to stdout, buffered by the kernel
        if (ret < 0) {
            handle_error("write", errno);
            ret = EXIT_FAILURE;
            goto err;
        }
    }
    /* parent section */
    else {
        pid_t pid = getpid();
        int wstatus;

        ret = close(pipefd[READ]); // closing the read part
        if (ret < 0) {
            handle_error("close", errno);
            ret = EXIT_FAILURE;
            goto err;
        }

        printf("[ppid:%d] closed the read end of pipe\n", pid);

        ret = write(pipefd[WRITE], *argv, strlen(*argv));
        if (ret < 0) {
            handle_error("write", errno);
            ret = EXIT_FAILURE;
            goto err;
        }

        ret = close(pipefd[WRITE]); // write part not needed anymore
        if (ret < 0) {
            handle_error("close", errno);
            ret = EXIT_FAILURE;
            goto err;
        }

        (void)wait(&wstatus);
        printf("[ppid:%d] waiting for %d done\n", pid, c_pid);
        if (WIFEXITED(wstatus)) {
            int exit_code = WEXITSTATUS(wstatus);
            printf("[ppid:%d] process %d exited %s\n", pid, c_pid, exit_code == 0 ? "normally" : "with an error");
        } else if (WIFSIGNALED(wstatus))
            printf("[ppid:%d] process %d killed by signal %d\n", pid, c_pid, WTERMSIG(wstatus));
    }

    ret = EXIT_SUCCESS;
err:
    return ret;
}
