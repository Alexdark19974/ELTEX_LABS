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
#define PIPE '|'
#define REDIR_INPUT '<'
#define REDIR_OUTPUT '>'
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)
#define handle_error_msg(fmt, ...) do { \
                            fprintf(stderr, fmt, __VA_ARGS__); \
                        } while(0)

typedef struct redir_e {
    char direct;
    char *file_name;
    char *cmd;
} redir_t;

int my_pipe(int cmd_num, char ***cmds, int redir_num, redir_t **to_from);

int main(int argc, char **argv)
{
    redir_t** to_from;
    char ***cmds = NULL;
    int i, j, l, k;
    int next_cmd_idx = 0;
    int pipe_num, arg_num, cmd_num, pipe_idx = 0;
    int redir_num, redir_idx = 0;
    int ret = EXIT_FAILURE;

    for (i = 1, pipe_num = 0, redir_num = 0; i < argc; i++) {
        if (*argv[i] == PIPE) {
            pipe_num++;
            pipe_idx = i;
        } else if (*argv[i] == REDIR_INPUT || *argv[i] == REDIR_OUTPUT) {
            redir_num++;
            redir_idx = i;
        }
    }

    if (pipe_idx == 1 || pipe_idx == argc - 1) {
        handle_error_msg("error: '%c' requires a command on both sides. You have %d\n", PIPE, pipe_idx);
        return EXIT_FAILURE;
    }

    if (redir_idx == 1 || redir_idx == argc - 1) {
        handle_error_msg("error: '%c' is used followed by an argument, i.e: prog %c arg or prog %c arg\n", *argv[redir_idx], REDIR_INPUT, REDIR_OUTPUT);
        return EXIT_FAILURE;
    }

    to_from = calloc(redir_num, sizeof(char *));
    if (!to_from) {
        if (!to_from) {
            handle_error("calloc", errno);
            goto err;
        }        
    }

    cmd_num = pipe_num + 1;
    cmds = calloc(cmd_num, sizeof(char **));
    if (!cmds) {
        handle_error("calloc", errno);
        goto err;
    }

    for (i = 0, l = 0, k = 0; i < cmd_num; i++)  {
        for (j = next_cmd_idx + 1, next_cmd_idx = j, arg_num = 0; j < argc && *argv[j] != PIPE && *argv[j] != REDIR_INPUT && *argv[j] != REDIR_OUTPUT; j++)
            arg_num++;
#if DEBUG
        printf("arg_num = %d\n", arg_num);
#endif
        if (arg_num > 0) {
            cmds[i] = calloc(arg_num + 1, sizeof(char **));
            if (!cmds[i]) {
                handle_error("calloc", errno);
                goto err;
            }
            for (l = 0, j = next_cmd_idx; l < arg_num; l++, j++) {
                cmds[i][l] = argv[j];
#if DEBUG
                // printf("argv[%d] = %s\n", j, argv[j]);
                printf("cmds[%d][%d] = %s\n", i, l, cmds[i][l]);
#endif
            }
            cmds[i][l] = NULL;
        }

        if (j < argc && (*argv[j] == PIPE || *argv[j] == REDIR_INPUT || *argv[j] == REDIR_OUTPUT)) {
            if (*argv[j] == REDIR_INPUT || *argv[j] == REDIR_OUTPUT) {
                int cnt_idx;
                while (j < argc && (*argv[j] == REDIR_INPUT || *argv[j] == REDIR_OUTPUT)) {
                    size_t size;

                    printf("'%c' is detected at argv[%d]\n", *argv[j], j);

                    to_from[k] = calloc(1, sizeof(redir_t));
                    if (!to_from[k]) {
                        handle_error("calloc", errno);
                        goto err;
                    }

                    to_from[k]->direct = *argv[j] == REDIR_OUTPUT ? WRITE : READ;
    
                    size = strlen(argv[j + 1]) + 1;
                    to_from[k]->file_name = calloc(1, size);
                    if (!to_from[k]->file_name) {
                        handle_error("calloc", errno);
                        goto err;
                    }
                    snprintf(to_from[k]->file_name, size, "%s", argv[j + 1]);

                    for (cnt_idx = j; cnt_idx > 0 && *argv[cnt_idx] != PIPE; cnt_idx--) ;

                    to_from[k]->cmd = argv[cnt_idx + 1];
#if DEBUG
                    printf("redirection attached to cmd '%s', current redir_idx = %d\n", to_from[k]->cmd, k);
#endif
                    j += 2, k++;
                }
            }
            next_cmd_idx = j;
#if DEBUG
            if (j + 1 < argc)
                printf("next cmd '%s' at argv[%d]\n", argv[j + 1], j);
#endif
        }
    }

#if DEBUG
    printf("cmd_num = %d\n", cmd_num);
    printf("redir_num = %d\n", redir_num);
#endif
    ret = my_pipe(cmd_num, cmds, redir_num, to_from);

err:
    if (cmds) {
        for (i = 0; i < cmd_num; i++)
            free(cmds[i]);
        free(cmds);
    }

    if (to_from) {
        for (l = 0; l < redir_num; l++)
            free(to_from[l]->file_name);
        free(to_from);
    }

    return ret;
}

int my_pipe(int cmd_num, char ***cmds, int redir_num, redir_t **to_from)
{
    pid_t *pid_list;
    int **pipe_list = NULL;
    int pipe_num;
    int prev_pipe_idx = 0, pipe_idx = 0;
    int ret = 0, wret = 0, i, j, l;
    bool is_redir = false;

    pid_list = calloc(cmd_num, sizeof(pid_t));
    if (!pid_list) {
        handle_error("calloc", errno);
        return EXIT_FAILURE;        
    }

    pipe_num = cmd_num - 1;
    if (pipe_num > 0) {
        pipe_list = calloc(pipe_num * PIPE_ENDS, sizeof(int *));
        if (!pipe_list) {
            handle_error("calloc", errno);
            goto free_mem;
        }

        for (i = 0, l = 0; i < pipe_num * PIPE_ENDS; i++) {
            pipe_list[i] = calloc(PIPE_ENDS, sizeof(int));
            if (!pipe_list[i]) {
                handle_error("calloc", errno);
                goto free_mem;
            }
        }
    }

    for (i = j = l = pipe_idx = prev_pipe_idx = 0; i < cmd_num; i++) {
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

        if (l < redir_num && to_from[l]->cmd == cmds[i][0]) {
#if DEBUG
            printf("to_from[%d]->cmd=%s, file_name=%s\n", l, to_from[l]->cmd, to_from[l]->file_name);
#endif
            is_redir = true;
        }

        pid_list[i] = fork();
#if DEBUG
        printf("pid[%d] = %d\n", i, pid_list[i]);
#endif
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
#if DEBUG
                        printf("[pid:%d] pipe_idx = %d, prev_pipe_idx = %d, pipe = %d, fd_num = %d, end = %d\n", getpid(), pipe_idx, prev_pipe_idx, pipe_list[prev_pipe_idx][WRITE], fd, end);
#endif
                        handle_error("close", errno);
                        exit(EXIT_FAILURE);
                    }
                }

                ret = close(pipe_list[prev_pipe_idx][READ]); // not needed at all
                if (ret < 0) {
#if DEBUG
                    printf("[pid:%d] pipe_idx = %d, prev_pipe_idx = %d, pipe = %d, fd_num = %d, end = %d\n", getpid(), pipe_idx, prev_pipe_idx, pipe_list[prev_pipe_idx][READ], fd, end);
#endif
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


            if (redir_num > 0 && is_redir == true) {
                int fd[2] = {-1}, fd_idx = 0;
                while (l < redir_num && to_from[l]->cmd == cmds[i][0]) {
                    if (to_from[l]->direct == WRITE)
                        fd[fd_idx] = open(to_from[l]->file_name, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
                    if (to_from[l]->direct == READ)
                        fd[fd_idx] = open(to_from[l]->file_name, O_RDONLY, 0);

                    if (fd[fd_idx] < 0) {
                        handle_error("open", errno);
                        for (fd_idx = 0; fd_idx < 2 && fd[fd_idx] > 0; fd_idx++)
                            (void)close(fd[fd_idx]);
                        exit(EXIT_FAILURE);
                    }

                    ret = dup2(fd[fd_idx], to_from[l]->direct);
                    if (ret < 0) {
                        handle_error("dup2", errno);
                        for (fd_idx = 0; fd_idx < 2 && fd[fd_idx] > 0; fd_idx++)
                            (void)close(fd[fd_idx]);
                        exit(EXIT_FAILURE);
                    }

                    l++;
                    fd_idx++;
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
#if DEBUG
            printf("prev_pipe_idx = %d, pipe_idx = %d\n", prev_pipe_idx, pipe_idx);
#endif
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

        if (redir_num > 0 && is_redir == true) {
            l++;
            is_redir = false;
        }
    }

err:
#if DEBUG
    for (i = 0; i < pipe_num; i++)
        printf("pipe_list[%d]: read end = %d, write end = %d\n", i, pipe_list[i][READ], pipe_list[i][WRITE]);
#endif
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
