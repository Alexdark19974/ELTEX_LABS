#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <stdbool.h>
#define READ    0
#define WRITE   1
#define PIPE_ENDS 2
#define BUF_SIZ 256
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)
#define handle_error_msg(fmt, ...) do { \
                            fprintf(stderr, fmt, __VA_ARGS__); \
                        } while(0)

int my_pipe(int cmd_num, char ***cmds);

int main(int argc, char **argv)
{
    int i, j, l;
    int next_cmd_idx;
    int pipe_num, arg_num, cmd_num, pipe_idx = 0;
    int ret = EXIT_FAILURE;
    char ***cmds;

    for (i = 1, pipe_num = 0; i < argc; i++) {
        if (*argv[i] == '|') {
            pipe_num++;
            pipe_idx = i;
        }
    }

    if (pipe_idx == 1 || pipe_idx == argc - 1) {
        handle_error_msg("error: '|' requires a command on both sides. You have %d\n", pipe_idx);
        return EXIT_FAILURE;
    }

    cmd_num = pipe_num + 1;
    printf("cmd_num = %d\n", cmd_num);

    cmds = calloc(cmd_num, sizeof(char **));
    if (!cmds) {
        handle_error("calloc", errno);
        goto err;
    }

    next_cmd_idx = 1;

    for (i = 0; i < cmd_num; i++) {
        for (j = next_cmd_idx, arg_num = 0; j < argc; j++) {
            if (*argv[j] == '|') {
                break;
            }
            arg_num++;
        }
        printf("arg_num = %d\n", arg_num);
        cmds[i] = calloc(arg_num + 1, sizeof(char **));
        if (!cmds[i]) {
            handle_error("calloc", errno);
            goto err;
        }
        for (l = 0, j = next_cmd_idx; l < arg_num; l++, j++) {
            cmds[i][l] = argv[j];
            printf("cmds[%d][%d] = %s\n", i, l, cmds[i][l]);
        }
        cmds[i][l] = NULL;
        next_cmd_idx = j + 1;
    }

    ret = my_pipe(cmd_num, cmds);

err:
    if (cmds) {
        for (i = 0; i < cmd_num; i++)
            free(cmds[i]);
        free(cmds);
    }

    return ret;
}

int my_pipe(int cmd_num, char ***cmds)
{
    pid_t *pid_list;
    int **pipe_list;
    int pipe_num;
    int prev_pipe_idx = 0, pipe_idx = 0;
    int ret = 0, wret = 0, i, j;

    pid_list = calloc(cmd_num, sizeof(pid_t));
    if (!pid_list) {
        handle_error("calloc", errno);
        return EXIT_FAILURE;        
    }

    pipe_num = cmd_num - 1;
    pipe_list = calloc(pipe_num * PIPE_ENDS, sizeof(int *));
    if (!pipe_list) {
        handle_error("calloc", errno);
        goto free_mem;
    }

    for (i = 0; i < pipe_num * PIPE_ENDS; i++) {
        pipe_list[i] = calloc(PIPE_ENDS, sizeof(int));
        if (!pipe_list[i]) {
            handle_error("calloc", errno);
            goto free_mem;
        }
    }

    for (i = j = pipe_idx = prev_pipe_idx = 0; i < cmd_num; i++) {
        if (pipe_num) {
            prev_pipe_idx = pipe_idx > 0 ? pipe_idx - 1 : pipe_idx;
            if (pipe_idx < pipe_num) {
                ret = pipe(pipe_list[pipe_idx]);
                if (ret < 0) {
                    handle_error("pipe", errno);
                    return EXIT_FAILURE;
                }
            }
        }

        pid_list[i] = fork();
        printf("pid[%d] = %d\n", i, pid_list[i]);
        if (pid_list[i] < 0) {
            handle_error("fork", errno);
            ret = EXIT_FAILURE;
            goto err;
        }

        /* child section */
        if (0 == pid_list[i]) {
            if (pipe_num) {
                int end = pipe_idx > 0 ? READ : WRITE; // READ = STDIN_FILENO = 0, WRITE = STDOUT_FILENO = 1
                int fd = end;
                ret = dup2(pipe_list[prev_pipe_idx][end], fd); // dup either FILENO, then close it
                if (ret < 0)  {
                    handle_error("dup2", errno);
                    exit(EXIT_FAILURE);
                }

                if (pipe_idx > 0 && pipe_idx < pipe_num) {
                    ret = dup2(pipe_list[pipe_idx][WRITE], STDOUT_FILENO); // dup stdout, then close it
                    if (ret < 0)  {
                        handle_error("dup2", errno);
                        exit(EXIT_FAILURE);
                    }
                }

                /* check if the pipe's write end was previously closed */
                if (pipe_list[prev_pipe_idx][WRITE] > 0) {
                    ret = close(pipe_list[prev_pipe_idx][WRITE]); // not needed after swap with stdout
                    if (ret < 0) {
                        printf("[pid:%d] pipe_idx = %d, prev_pipe_idx = %d, pipe = %d, fd_num = %d, end = %d\n", getpid(), pipe_idx, prev_pipe_idx, pipe_list[prev_pipe_idx][WRITE], fd, end);
                        handle_error("close", errno);
                        exit(EXIT_FAILURE);
                    }
                }

                ret = close(pipe_list[prev_pipe_idx][READ]); // not needed at all
                if (ret < 0) {
                    printf("[pid:%d] pipe_idx = %d, prev_pipe_idx = %d, pipe = %d, fd_num = %d, end = %d\n", getpid(), pipe_idx, prev_pipe_idx, pipe_list[prev_pipe_idx][READ], fd, end);
                    handle_error("close", errno);
                    exit(EXIT_FAILURE);
                }

                if (pipe_idx > 0 && pipe_idx < pipe_num) {
                    ret = close(pipe_list[pipe_idx][WRITE]); // not needed after swap with stdout
                    if (ret < 0) {
                        handle_error("close", errno);
                        exit(EXIT_FAILURE);
                    }

                    ret = close(pipe_list[pipe_idx][READ]); // not needed at all
                    if (ret < 0) {
                        handle_error("close", errno);
                        exit(EXIT_FAILURE);
                    }
                }
            }
            /* the output is written to stdout, which has been reopened as pipefd[WRITE] */
            /* stdout = pipefd[WRITE] */
            ret = execvp(cmds[i][0], cmds[i]);
            if (ret < 0) { // this should never happen
                handle_error("execlp", errno);
                exit(EXIT_FAILURE);
            }
        }

        /* ls (stdout = pipe1) | (stdin = pipe1) head */
        /* ls (stdout = pipe1) | (stdin = pipe1) head (stdout = pipe2) | (stdin = pipe2) nl */
        if (pipe_num) {
            printf("prev_pipe_idx = %d, pipe_idx = %d\n", prev_pipe_idx, pipe_idx);
            if (pipe_idx < pipe_num) {
                (void)close(pipe_list[pipe_idx][WRITE]); // not needed at all
                pipe_list[pipe_idx][WRITE] = -1;
            }
            if (pipe_idx > 0) {
                (void)close(pipe_list[prev_pipe_idx][READ]); // not needed at all
                pipe_list[prev_pipe_idx][READ] = -1;
            }
            pipe_idx++;
        }
    }

err:
    for (i = 0; i < pipe_num; i++)
        printf("pipe_list[%d]: read end = %d, write end = %d\n", i, pipe_list[i][READ], pipe_list[i][WRITE]);
    /* parent section */
    for (j = 0; j < cmd_num; j++) {
        if (pid_list[j] > 0) {
            if (ret == EXIT_FAILURE)
                (void)kill(pid_list[j], SIGKILL);
            wret = waitpid(pid_list[j], &ret, 0);
            if (wret < 0) {
                handle_error("waitpid", errno);
                break;
            }
            if (WIFEXITED(ret)) 
                printf("exited, status=%d\n", WEXITSTATUS(ret));
            else if (WIFSIGNALED(ret)) 
                printf("killed by signal %d\n", WTERMSIG(ret));
        }
    }

free_mem:
    if (pipe_list) {
        for (i = 0; i < pipe_num * PIPE_ENDS; i++)
            free(pipe_list[i]);
        free(pipe_list);
    }

    free(pid_list);

    return ret;
}
