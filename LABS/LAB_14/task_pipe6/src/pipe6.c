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

typedef struct cmd_ctx_s {
    char ***cmds;
    int **pipe_list;
    pid_t *pid_list;
    redir_t **to_from;
    int cmd_num;
    int pipe_num;
    int redir_num;
} cmd_ctx_t;

int exec_cmd_list(cmd_ctx_t cmd_ctx);

int main(int argc, char **argv)
{
    int i, j, l, k;
    int next_cmd_idx = 0, redir_idx = 0, pipe_idx = 0;
    int pipe_num, arg_num, redir_num;
    int ret = EXIT_FAILURE;
    cmd_ctx_t cmd_ctx;

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
        handle_error_msg("error: '%c' requires a command on both sides. You have %d\n",
            PIPE, pipe_idx);
        return EXIT_FAILURE;
    }

    if (redir_idx == 1 || redir_idx == argc - 1) {
        handle_error_msg("error: '%c' is used followed by an argument, i.e: prog %c arg or prog %c arg\n",
            *argv[redir_idx], REDIR_INPUT, REDIR_OUTPUT);
        return EXIT_FAILURE;
    }

    memset(&cmd_ctx, 0x0, sizeof(cmd_ctx));
    cmd_ctx.pipe_num = pipe_num;
    cmd_ctx.cmd_num = pipe_num + 1;
    cmd_ctx.redir_num = redir_num;

    if (cmd_ctx.redir_num) {
        cmd_ctx.to_from = calloc(redir_num, sizeof(char *));
        if (!cmd_ctx.to_from) {
            handle_error("calloc", errno);
            goto err;
        }        
    }

    if (cmd_ctx.pipe_num > 0) {
        cmd_ctx.pipe_list = calloc(pipe_num * PIPE_ENDS, sizeof(int *));
        if (!cmd_ctx.pipe_list) {
            handle_error("calloc", errno);
            goto err;
        }

        for (i = 0; i < cmd_ctx.pipe_num * PIPE_ENDS; i++) {
            cmd_ctx.pipe_list[i] = calloc(PIPE_ENDS, sizeof(int));
            if (!cmd_ctx.pipe_list[i]) {
                handle_error("calloc", errno);
                goto err;
            }
        }
    }

    cmd_ctx.pid_list = calloc(cmd_ctx.cmd_num, sizeof(pid_t));
    if (!cmd_ctx.pid_list) {
        handle_error("calloc", errno);
        goto err;        
    }

    cmd_ctx.cmds = calloc(cmd_ctx.cmd_num, sizeof(char **));
    if (!cmd_ctx.cmds) {
        handle_error("calloc", errno);
        goto err;
    }

    for (i = 0, l = 0, k = 0; i < cmd_ctx.cmd_num; i++)  {
        for (j = next_cmd_idx + 1, next_cmd_idx = j, arg_num = 0;
            j < argc && *argv[j] != PIPE && *argv[j] != REDIR_INPUT && *argv[j] != REDIR_OUTPUT; j++)
            arg_num++;
#if DEBUG
        printf("arg_num = %d\n", arg_num);
#endif
        if (arg_num > 0) {
            cmd_ctx.cmds[i] = calloc(arg_num + 1, sizeof(char **));
            if (!cmd_ctx.cmds[i]) {
                handle_error("calloc", errno);
                goto err;
            }
            for (l = 0, j = next_cmd_idx; l < arg_num; l++, j++) {
                cmd_ctx.cmds[i][l] = argv[j];
#if DEBUG
                printf("cmds[%d][%d] = %s\n", i, l, cmd_ctx.cmds[i][l]);
#endif
            }
            cmd_ctx.cmds[i][l] = NULL;
        }

        if (j < argc && (*argv[j] == PIPE || *argv[j] == REDIR_INPUT || *argv[j] == REDIR_OUTPUT)) {
            if (*argv[j] == REDIR_INPUT || *argv[j] == REDIR_OUTPUT) {
                int cnt_idx;
                while (j < argc && (*argv[j] == REDIR_INPUT || *argv[j] == REDIR_OUTPUT)) {
                    size_t size;

                    printf("'%c' is detected at argv[%d]\n", *argv[j], j);

                    cmd_ctx.to_from[k] = calloc(1, sizeof(redir_t));
                    if (!cmd_ctx.to_from[k]) {
                        handle_error("calloc", errno);
                        goto err;
                    }

                    cmd_ctx.to_from[k]->direct = *argv[j] == REDIR_OUTPUT ? WRITE : READ;
    
                    size = strlen(argv[j + 1]) + 1;
                    cmd_ctx.to_from[k]->file_name = calloc(1, size);
                    if (!cmd_ctx.to_from[k]->file_name) {
                        handle_error("calloc", errno);
                        goto err;
                    }
                    snprintf(cmd_ctx.to_from[k]->file_name, size, "%s", argv[j + 1]);

                    for (cnt_idx = j; cnt_idx > 0 && *argv[cnt_idx] != PIPE; cnt_idx--) ;

                    cmd_ctx.to_from[k]->cmd = argv[cnt_idx + 1];
#if DEBUG
                    printf("redirection attached to cmd '%s', current redir_idx = %d\n",
                        cmd_ctx.to_from[k]->cmd, k);
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
    printf("cmd_num = %d\n", cmd_ctx.cmd_num);
    printf("pipe_num = %d\n", cmd_ctx.pipe_num);
    printf("redir_num = %d\n", cmd_ctx.redir_num);
#endif
    ret = exec_cmd_list(cmd_ctx);

err:
    if (cmd_ctx.redir_num > 0) {
        for (i = 0; i < redir_num; i++)
            free(cmd_ctx.to_from[i]->file_name);
        free(cmd_ctx.to_from);
    }

    if (cmd_ctx.pipe_num > 0) {
        for (i = 0; i < cmd_ctx.pipe_num * PIPE_ENDS; i++)
            free(cmd_ctx.pipe_list[i]);
        free(cmd_ctx.pipe_list);
    }

    free(cmd_ctx.pid_list);

    if (cmd_ctx.cmd_num > 0) {
        for (i = 0; i < cmd_ctx.cmd_num; i++)
            free(cmd_ctx.cmds[i]);
        free(cmd_ctx.cmds);
    }

    return ret;
}

int exec_cmd_list(cmd_ctx_t cmd_ctx)
{
    int *pid_list = cmd_ctx.pid_list;
    int **pipe_list = cmd_ctx.pipe_list;
    redir_t **to_from = cmd_ctx.to_from;
    char ***cmds = cmd_ctx.cmds;
    int cmd_num = cmd_ctx.cmd_num;
    int pipe_num = cmd_ctx.pipe_num;
    int redir_num = cmd_ctx.redir_num;
    int prev_pipe_idx = 0, pipe_idx = 0, redir_idx = 0;
    int ret = 0, wret = 0, i, j;

    for (i = j = pipe_idx = prev_pipe_idx = 0; i < cmd_num; i++) {
        if (pipe_num > 0) {
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
        if (pid_list[i] < 0) {
            handle_error("fork", errno);
            ret = EXIT_FAILURE;
            goto err;
        }

        /* child section */
        if (0 == pid_list[i]) {
            if (pipe_num > 0 || redir_num > 0) {
                int end = WRITE;
                int fds[2] = {-1};
                int fd_idx = 0;

                if (pipe_idx > 0)
                    end = READ ; // READ = STDIN_FILENO = 0, WRITE = STDOUT_FILENO = 1

                if (pipe_num > 0 && pipe_list[prev_pipe_idx][end] > 0) {
                    printf("[%s:%d] dup2 pipe %d %s end, cmd=%s, fd=%d, i=%d\n",
                         __func__, __LINE__, pipe_list[prev_pipe_idx][end], end == WRITE ? "write" : "read", cmds[i][0],
                         fds[fd_idx], i);
                    ret = dup2(pipe_list[prev_pipe_idx][end], end); // dup either FILENO, then close it
                    if (ret < 0)  {
                        handle_error("dup2", errno);
                        exit(EXIT_FAILURE);
                    }
                }
                if (redir_idx < redir_num && to_from[redir_idx]->cmd == cmds[i][0]) {
                    if (to_from[redir_idx]->direct == WRITE)
                        fds[fd_idx] = open(to_from[redir_idx]->file_name, 
                            O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
                    else if (to_from[redir_idx]->direct == READ)
                        fds[fd_idx] = open(to_from[redir_idx]->file_name, O_RDONLY, 0);
                    if (fds[fd_idx] < 0) {
                        handle_error("open", errno);
                        (void)close(fds[fd_idx]);
                        exit(EXIT_FAILURE);
                    }
#if DEBUG
                    printf("[%s:%d] dup2 %s, cmd=%p, cmds=%p, direction=%d, fd=%d, i=%d\n", __func__, __LINE__,
                        to_from[redir_idx]->file_name, (void *)to_from[redir_idx]->cmd, (void *)cmds[i][0],
                        to_from[redir_idx]->direct, fds[fd_idx], i);
#endif                    
                    ret = dup2(fds[fd_idx], to_from[redir_idx]->direct);
                    if (ret < 0) {
                        handle_error("dup2", errno);
                        (void)close(fds[fd_idx]);
                        exit(EXIT_FAILURE);
                    }
                    fd_idx++;
                    redir_idx++;
                }

                if (pipe_idx > 0 && pipe_idx < pipe_num) {
                    ret = dup2(pipe_list[pipe_idx][WRITE], STDOUT_FILENO); // dup stdout, then close it
                    if (ret < 0)  {
                        handle_error("dup2", errno);
                        exit(EXIT_FAILURE);
                    }
                }

                if (redir_idx > 0 && redir_idx < redir_num && to_from[redir_idx]->cmd == cmds[i][0]) {
                    fds[fd_idx] = open(to_from[redir_idx]->file_name, 
                        O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
#if DEBUG
                    printf("[%s:%d] dup2 %s, cmd=%p, cmds=%p, direction=%d, fd=%d, i=%d\n", __func__, __LINE__,
                        to_from[redir_idx]->file_name, (void *)to_from[redir_idx]->cmd, (void *)cmds[i][0],
                        to_from[redir_idx]->direct, fds[fd_idx], i);
#endif
                    ret = dup2(fds[fd_idx], to_from[redir_idx]->direct);
                    if (ret < 0) {
                        handle_error("dup2", errno);
                        for (fd_idx = 0; fd_idx < 2 && fds[fd_idx] > 0; fd_idx++)
                            (void)close(fds[fd_idx]);
                        exit(EXIT_FAILURE);
                    }
                }

                /* check if the pipe's write end was previously closed */
                if (pipe_num > 0 && pipe_list[prev_pipe_idx][WRITE] > 0) {
                    ret = close(pipe_list[prev_pipe_idx][WRITE]); // not needed after swap with stdout
                    if (ret < 0) {
#if DEBUG
                        printf("[pid:%d] pipe_idx = %d, prev_pipe_idx = %d, pipe = %d, fd_num = %d, end = %d\n",
                            getpid(), pipe_idx, prev_pipe_idx, pipe_list[prev_pipe_idx][WRITE], end, end);
#endif
                        handle_error("close", errno);
                        exit(EXIT_FAILURE);
                    }
                }

                if (pipe_num > 0) {
                    ret = close(pipe_list[prev_pipe_idx][READ]); // not needed at all
                    if (ret < 0) {
#if DEBUG
                        printf("[pid:%d] pipe_idx = %d, prev_pipe_idx = %d, pipe = %d, fd_num = %d, end = %d\n",
                            getpid(), pipe_idx, prev_pipe_idx, pipe_list[prev_pipe_idx][READ], end, end);
#endif
                        handle_error("close", errno);
                        exit(EXIT_FAILURE);
                    }
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
        if (pipe_num > 0) {
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

        if (redir_idx < redir_num) {
            if (to_from[redir_idx]->cmd == cmds[i][0])
                redir_idx++;
            if (redir_idx + 1 < redir_num && to_from[redir_idx + 1]->cmd == cmds[i][0])
                redir_idx++;

#if DEBUG
            printf("redir_idx = %d\n", redir_idx);
#endif
        }
    }

err:
#if DEBUG
    for (i = 0; i < pipe_num; i++)
        printf("pipe_list[%d]: read end = %d, write end = %d\n", i,
            pipe_list[i][READ], pipe_list[i][WRITE]);
#endif
    /* parent section */
    for (i = 0; i < cmd_num; i++) {
        if (pid_list[i] > 0) {
            if (ret == EXIT_FAILURE)
                (void)kill(pid_list[i], SIGKILL);
            wret = waitpid(pid_list[i], &ret, 0);
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

    return ret;
}
