#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <malloc.h>
#include <sys/types.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

typedef enum act {
    FAIL = -1,
    OK = 0,
    QUIT = 1,
    CHDIR = 2,
} act_t;

#define CMD_SIZE (128)
#define ARG_NUM 24
#define NEWLINE ('\n')
#define IS_NEWLINE(x) ((x) == NEWLINE)
#define DELIMS ";&&"
#define DELIMS_2 "|<>"
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

int pseudo_bash(void);
int parse_cmd(char *buf);
int arg_count(const char *buf);
void sighandler(int signo, siginfo_t *info, void *context);
int exec_cmd_list(cmd_ctx_t cmd_ctx);

int main(void)
{
    struct sigaction act = {0};
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    act.sa_sigaction = &sighandler;
    if (sigaction(SIGINT, &act, NULL) == FAIL) {
        handle_error("sigaction", errno);
        return EXIT_FAILURE;        
    }
    return pseudo_bash();
}

int pseudo_bash(void)
{
    char *chp = NULL;
    const char *user = NULL;
    char hostname[HOST_NAME_MAX + 1] = {0};
    int ret = EXIT_FAILURE;
    char buf[BUFSIZ + 1] = {0}, dir[BUFSIZ + 1] = {0};
    bool loop = true;

    if (!(user = getenv("USER"))) {
        handle_error("getenv", errno);
        goto err;
    } else if (gethostname(hostname, sizeof(hostname)) < 0) {
        handle_error("gethostname", errno);
        goto err;       
    }
 
    while (loop) {
        if (!*dir && !getcwd(dir, sizeof(dir)))
                goto err;

        printf("[%s@%s:%s] $ ", user, hostname, dir);
        fflush(stdout); // fgets (stdin) usually triggers fflush() but, on some systems, it doesn't
        if (!fgets(buf, sizeof(buf), stdin)) {
            if (ferror(stdin)) {
                handle_error("fgets", errno);
                break;
            } else if (feof(stdin)) { // leave at the 1st sight of EOF
                printf("\nreceived EOF from input\n");
                loop = false;
            }
            continue;
        }

        if (IS_NEWLINE(*buf))
            continue;

        if (!(chp = strchr(buf, NEWLINE))) { // input sanitization: discard if buf is not large enough
            printf("[%s:%d] WARNING: input was truncated because it's larger than %d bytes. Discarding the rest\n", __func__, __LINE__, BUFSIZ);
            while (getchar() != '\n') ;
            continue;
        }
        *chp = '\0'; // parse_cmd needs no newlines

        switch ((ret = parse_cmd(buf))) {
            case QUIT:
                ret = EXIT_SUCCESS;
                loop = false;
                break;
            case FAIL:
                ret = EXIT_FAILURE;
                loop = false;
                break;
            case CHDIR:
                *dir = '\0';
                break;
            default:
                break;
        }

        memset(buf, 0x0, sizeof(buf));
    }

err:
    return ret;
}

int parse_cmd(char *buf)
{
    char *tok = NULL, *cmd = NULL;
    int ret = OK;

    tok = strtok(buf, DELIMS);
    while (tok) { // while loop is more appropriate than do-while for strtok: redunancy, clean code, etc.
#if DEBUG
        printf("tok=%s\n", tok);
#endif
        while (isspace(*tok)) tok++;

        if (!strcmp(tok, "exit")) {
            ret = QUIT;
            break;
        } else if (!strncmp(tok, "cd", 2)) {
            char *home = NULL;
            tok += 2;
            if (!*tok) {
home:
                home = getenv("HOME");
                if (!home) {
                    handle_error_msg("%s var is missing!", "$HOME");
                    ret = FAIL;
                    break;
                }
            } else {
                char *sub_tok = NULL;
                while (isspace(*tok)) tok++;
                if (!*tok)
                    goto home;
                sub_tok = tok;
                while (*sub_tok && !isspace(*sub_tok)) sub_tok++;
                *sub_tok = '\0';
            }
#if DEBUG
            printf("arg=%s\n", tok);
#endif
            ret = chdir(*tok ? tok : home) < 0 ? FAIL : CHDIR;
            if (ret == FAIL)
                handle_error("chdir", errno);
            goto next;
        }

        free(cmd);
        cmd = strdup(tok);
        if (!cmd) {
            handle_error("strdup", errno);
            return FAIL;
        }

#if DEBUG
        printf("cmd: %s\n", cmd);
#endif
        {
            char **argv = NULL;
            cmd_ctx_t cmd_ctx;
            int i, j, l, k;
            int next_cmd_idx = 0, redir_idx = -1, pipe_idx = -1;
            int pipe_num, arg_num, redir_num;
            int argc;

            memset(&cmd_ctx, 0x0, sizeof(cmd_ctx));

            argc = arg_count(cmd) + 1; /* extra argv[i] for execvp */
#if DEBUG
            printf("argc=%d\n", argc);
#endif
            argv = calloc(argc, sizeof(char *));
            if (!argv) {
                handle_error("calloc", errno);
                ret = FAIL;
                break;
            }

            i = 0;
            char *cmd_tok = strtok(cmd, " ");
            while (cmd_tok) {
                argv[i] = cmd_tok;
#if DEBUG
                printf("arg=%s\n", argv[i]);
#endif
                i++;
                cmd_tok = strtok(NULL, " ");
            }

            for (i = 0, pipe_num = 0, redir_num = 0; i < argc; i++) {
                if (*argv[i] == PIPE) {
                    pipe_num++;
                    pipe_idx = i;
                } else if (*argv[i] == REDIR_INPUT || *argv[i] == REDIR_OUTPUT) {
                    redir_num++;
                    redir_idx = i;
                }
            }

            if (pipe_idx == 0 || pipe_idx == argc - 1) {
                handle_error_msg("error: '%c' requires a command on both sides. You have %d\n",
                    PIPE, pipe_idx);
                goto err;
            }

            if (redir_idx == 0 || redir_idx == argc - 1) {
                handle_error_msg("error: '%c' is used followed by an argument, i.e: prog %c arg or prog %c arg\n",
                    *argv[redir_idx], REDIR_INPUT, REDIR_OUTPUT);
                goto err;
            }

            cmd_ctx.pipe_num = pipe_num;
            cmd_ctx.cmd_num = pipe_num + 1;
            cmd_ctx.redir_num = redir_num;

            if (cmd_ctx.redir_num > 0) {
                cmd_ctx.to_from = calloc(cmd_ctx.redir_num, sizeof(char *));
                if (!cmd_ctx.to_from) {
                    handle_error("calloc", errno);
                    ret = FAIL;
                    goto err;
                }
            }

            if (cmd_ctx.pipe_num > 0) {
                cmd_ctx.pipe_list = calloc(cmd_ctx.pipe_num * PIPE_ENDS, sizeof(int *));
                if (!cmd_ctx.pipe_list) {
                    handle_error("calloc", errno);
                    ret = FAIL;
                    goto err;
                }

                for (i = 0; i < cmd_ctx.pipe_num * PIPE_ENDS; i++) {
                    cmd_ctx.pipe_list[i] = calloc(PIPE_ENDS, sizeof(int));
                    if (!cmd_ctx.pipe_list[i]) {
                        handle_error("calloc", errno);
                        ret = FAIL;
                        goto err;
                    }
                }
            }

            cmd_ctx.pid_list = calloc(cmd_ctx.cmd_num, sizeof(pid_t));
            if (!cmd_ctx.pid_list) {
                handle_error("calloc", errno);
                ret = FAIL;
                goto err;
            }

            cmd_ctx.cmds = calloc(cmd_ctx.cmd_num, sizeof(char **));
            if (!cmd_ctx.cmds) {
                handle_error("calloc", errno);
                ret = FAIL;
                goto err;
            }

            for (i = 0, l = 0, k = 0; i < cmd_ctx.cmd_num; i++)  {
                for (j = next_cmd_idx, next_cmd_idx = j, arg_num = 0;
                    j < argc && *argv[j] != '\0' && *argv[j] != PIPE && *argv[j] != REDIR_INPUT && *argv[j] != REDIR_OUTPUT; j++)
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
#if DEBUG
                            printf("'%c' is detected at argv[%d]\n", *argv[j], j);
#endif
                            cmd_ctx.to_from[k] = calloc(1, sizeof(redir_t));
                            if (!cmd_ctx.to_from[k]) {
                                handle_error("calloc", errno);
                                ret = FAIL;
                                goto err;
                            }

                            cmd_ctx.to_from[k]->direct = *argv[j] == REDIR_OUTPUT ? WRITE : READ;
            
                            size = strlen(argv[j + 1]) + 1;
                            cmd_ctx.to_from[k]->file_name = calloc(1, size);
                            if (!cmd_ctx.to_from[k]->file_name) {
                                handle_error("calloc", errno);
                                ret = FAIL;
                                goto err;
                            }
                            snprintf(cmd_ctx.to_from[k]->file_name, size, "%s", argv[j + 1]);

                            for (cnt_idx = j; cnt_idx > 0 && *argv[cnt_idx] != PIPE; cnt_idx--) ;

                            cmd_ctx.to_from[k]->cmd = *argv[cnt_idx] == PIPE ? argv[cnt_idx + 1] : argv[cnt_idx];
#if DEBUG
                            printf("redirection (%s) attached to cmd '%s', current redir_idx = %d\n",
                                cmd_ctx.to_from[k]->direct == WRITE ? "write" : "read", cmd_ctx.to_from[k]->cmd, k);
#endif
                            j += 2, k++;
                        }
                    }
                    next_cmd_idx = j + 1;
#if DEBUG
                    if (j + 1 < argc)
                        printf("next cmd '%s' at argv[%d]\n", argv[j + 1], j + 1);
#endif
                }
            }

#if DEBUG
            printf("cmd_num = %d\n", cmd_ctx.cmd_num);
            printf("pipe_num = %d\n", cmd_ctx.pipe_num);
            printf("redir_num = %d\n", cmd_ctx.redir_num);
#endif
            (void)signal(SIGINT, SIG_DFL);
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

            if (ret == FAIL)
                break;

            if (argv) {
                free(argv);
                argv = NULL;
            }
        }
next:
        tok = strtok(NULL, DELIMS);
    }

    free(cmd);

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
#if DEBUG
                printf("[%s:%d:%d] redir_idx=%d, pipe_idx=%d, i=%d\n", __func__, __LINE__, getpid(), redir_idx, pipe_idx, i);
#endif   
                if (pipe_num > 0 && pipe_list[prev_pipe_idx][end] > 0) {
                    printf("[%s:%d:%d] dup2 pipe %d %s end, cmd=%s, fd=%d, i=%d\n",
                         __func__, __LINE__, getpid(), pipe_list[prev_pipe_idx][end], end == WRITE ? "write" : "read", cmds[i][0],
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
                    printf("[%s:%d:%d] dup2 %s, cmd=%p, cmds=%p, direction=%d, fd=%d, i=%d\n", __func__, __LINE__, getpid(),
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
                    printf("[%s:%d:%d] dup2 %s, cmd=%p, cmds=%p, direction=%d, fd=%d, i=%d\n", __func__, __LINE__, getpid(),
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
                        printf("[%s:%d:%d] pipe_idx = %d, prev_pipe_idx = %d, pipe = %d, fd_num = %d, end = %d\n",
                            __func__, __LINE__, getpid(), pipe_idx, prev_pipe_idx, pipe_list[prev_pipe_idx][WRITE], end, end);
#endif
                        handle_error("close", errno);
                        exit(EXIT_FAILURE);
                    }
                }

                if (pipe_num > 0) {
                    ret = close(pipe_list[prev_pipe_idx][READ]); // not needed at all
                    if (ret < 0) {
#if DEBUG
                        printf("[%s:%d:%d] pipe_idx = %d, prev_pipe_idx = %d, pipe = %d, fd_num = %d, end = %d\n",
                            __func__, __LINE__, getpid(), pipe_idx, prev_pipe_idx, pipe_list[prev_pipe_idx][READ], end, end);
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
            if (pipe_idx > 0 && prev_pipe_idx < pipe_num) {
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

int arg_count(const char *buf)
{
    int cnt = 0;

    if (!buf || !*buf)
        return cnt;

    /* we skip the first trailing spaces before counting */
    while (*buf && (isspace(*buf))) buf++;

    while (*buf) {
        if (isspace(*buf)) {
            while (*buf && (isspace(*buf))) buf++;
            if (!*buf)
                break;
            cnt++;
        }
        buf++;
    }
#if DEBUG
    printf("cnt=%d\n", cnt);
#endif
    return cnt;
}

void sighandler(int signo, siginfo_t *info, void *context)
{
    (void)info;
    (void)context;

    printf("received sig%d\n", signo);
}