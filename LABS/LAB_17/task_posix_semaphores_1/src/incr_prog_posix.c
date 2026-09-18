#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <semaphore.h>
#include <signal.h>
#include <limits.h>
#define DEFAULT_INCR_MAX_NUM 20
#define DEFAULT_PROC_NUM 2
#define EXAMPLE_FILENAME "/file_name"
#define FILE_NAME_OPT 'f'
#define INCR_OPT 'i'
#define PROC_OPT 'p'
#define HELP_OPT 'h'
#define SLASH '/'
#define BUF_SIZ 256
#define PERMS 0666 // -rw-rw-rw
#define INCR_IDX 0
#define CAP_IDX 1
#define SEM_VAL 1
#define SEM_PREFIX "sem."
#define SEM_PREFIX_LEN 4
#define SHM_SUFFIX "_shm"
#define SEM_SUFFIX "_sem"
#define SUFFIX_LEN 4
#define handle_error(msg, errnum) do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = NULL; \
                                    if (!(err_str = strerror_r((errnum), buf, sizeof(buf)))) \
                                        fprintf(stderr, "error: failed to print error message!\n"); \
                                    else \
                                        fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)
#define handle_error_msg(fmt, ...) do { \
                            fprintf(stderr, fmt, __VA_ARGS__); \
                        } while(0)

void print_usage(const char *prog_name);
bool options_bundled(int opt, const char *optarg);
void incr_subroutine(const char *sem_name, const char *shm_name, size_t memseg_size);

int main(int argc, char **argv)
{
    const char *file_name = NULL, *prog_name = NULL;
    char shm_name[BUF_SIZ] = {0}, sem_name[BUF_SIZ] = {0};
    void *memseg = NULL;
    int *incr_val = NULL;
    sem_t *smphr = NULL;
    size_t memseg_size = 0;
    size_t len = 0;
    long num = 0;
    int opt = 0, wret = 0, status = EXIT_SUCCESS;
    int shmfd = -1;
    int i = 0, incr_num = 0, proc_num = 0;
    pid_t *pid_list = NULL;

    prog_name = argv[0];

    while ((opt = getopt(argc, argv, ":hf:i:p:")) != -1) {
        switch (opt) {
            case FILE_NAME_OPT:
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                file_name = optarg;
                len = strlen(file_name);
                /* NAME_MAX is max length for POSIX shared memory name */
                /* NAME_MAX - 4 is max length for POSIX semaphore name */
                if (((int)len > NAME_MAX - SEM_PREFIX_LEN - SUFFIX_LEN)) { // NAME_MAX - 4 - 4 = 247 to account for _shm suffix (4) / prefix "sem." + null-terminator 
                    handle_error_msg("File name cannot be longer than %d characters. Your number of characters is %zu\n", NAME_MAX - SEM_PREFIX_LEN - SUFFIX_LEN, len);
                    goto print;
                }
                break;
            case HELP_OPT:
                goto print;
            case INCR_OPT:
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                errno = 0;
                num = strtol(optarg, NULL, 10);
                if (errno != 0) {
                    handle_error("strtol", errno);
                    status = EXIT_FAILURE;
                    goto print;
                } else if (num <= 0) {
                    handle_error_msg("Number of increments must be a strictly positive integer. Your number is %ld\n", num);
                    goto print;
                }
                incr_num = (int)num;
                break;
            case PROC_OPT:
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                errno = 0;
                num = strtol(optarg, NULL, 10);
                if (errno != 0) {
                    handle_error("strtol", errno);
                    status = EXIT_FAILURE;
                    goto print;
                } else if (num <= 0) {
                    handle_error_msg("Number of processes must be a strictly positive integer. Your number is %ld\n", num);
                    goto print;
                }
                proc_num = (int)num;
                break;
            case ':':
                if (optopt == FILE_NAME_OPT)
                    handle_error_msg("-%c requires an argument, e.g. %s\n", optopt, EXAMPLE_FILENAME);
                else if (optopt == INCR_OPT)
                    handle_error_msg("-%c requires an argument, e.g. 4\n", optopt);
                else if (optopt == PROC_OPT)
                    handle_error_msg("-%c requires an argument, e.g. 4\n", optopt);
                goto print;
            default: // '?'
                handle_error_msg("-%c is an unknown option\n", optopt);
                goto print;
        }
    }

    if (!file_name) {
        handle_error_msg("file for shared memory is not specified! Please specify a file name, e.g. '%s'\n",
            EXAMPLE_FILENAME);
        goto print;
    } else if (*file_name != SLASH) {
        handle_error_msg("file name is missing an inital slash. Append to your file name, e.g. '%s'\n",
            EXAMPLE_FILENAME);
        goto print;
    }

    if (!incr_num) {
        printf("Number of increments is not specified. Defaulting to %d\n", DEFAULT_INCR_MAX_NUM);
        incr_num = DEFAULT_INCR_MAX_NUM;
    }

    if (!proc_num) {
        printf("Number of processes is not specified. Defaulting to %d\n", DEFAULT_PROC_NUM);
        proc_num = DEFAULT_PROC_NUM;
    }

    printf("file name = %s\n", file_name);

    if (!(pid_list = calloc((size_t)proc_num, sizeof(pid_t)))) {
        handle_error("calloc", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    snprintf(shm_name, sizeof(shm_name), "%s%s", file_name, SHM_SUFFIX);
    printf("shared memory file name = %s\n", shm_name);

    memseg_size = sizeof(int) + sizeof(int);
    /* create a handle (i.e. a file descriptor) to manage a shared memory object */
    if ((shmfd = shm_open(shm_name, O_CREAT | O_RDWR | O_TRUNC, PERMS)) < 0) {
        handle_error("shm_open", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    /* ftruncate sets the shared memory object's ("file") size to nbytes */
    if (ftruncate(shmfd, memseg_size) < 0) {
        handle_error("ftruncate", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    if ((memseg = mmap(NULL, memseg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0)) == (void *)-1) {
        handle_error("mmap", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    (void)close(shmfd);
    shmfd = -1;

    incr_val = (int *)memseg;
    incr_val[INCR_IDX] = 0; // set it to 0 explicitly
    incr_val[CAP_IDX] = incr_num;

    printf("incr_val = %d, incr_cap = %d\n", incr_val[INCR_IDX], incr_val[CAP_IDX]);    

    snprintf(sem_name, sizeof(sem_name), "%s%s", file_name, SEM_SUFFIX);
    printf("semaphore file name = %s\n", sem_name);

    if ((smphr = sem_open(sem_name, O_CREAT | O_EXCL | O_RDWR, PERMS, SEM_VAL)) == SEM_FAILED) {
        handle_error("sem_open", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    for (i = 0; i < proc_num; i++) {
        if ((pid_list[i] = fork()) < 0) {
            handle_error("fork", errno);
            status = EXIT_FAILURE;
            goto end;        
        }

        if (pid_list[i] == 0) // child section
            incr_subroutine(sem_name, shm_name, memseg_size);
    }

end:
    /* parent section */
    if (pid_list) {
        for (i = 0; i < proc_num; i++) {
            if (pid_list[i] > 0) {
                if (status == EXIT_FAILURE)
                    (void)kill(pid_list[i], SIGKILL);
                if (waitpid(pid_list[i], &wret, 0) < 0) {
                    status = EXIT_FAILURE;
                    handle_error("waitpid", errno);
                    continue;
                }
                if (WIFEXITED(wret)) 
                    printf("exited, wret = %d\n", WEXITSTATUS(wret));
                else if (WIFSIGNALED(wret)) 
                    printf("killed by signal %d\n", WTERMSIG(wret));
                pid_list[i] = -1;
            }
        }
        free(pid_list);
    }

    if (incr_val)
        printf("incr val = %d\n", incr_val[INCR_IDX]);

    if (memseg && munmap(memseg, memseg_size) < 0) {
        handle_error("munmap", errno);
        status = EXIT_FAILURE;
    }

    if (shmfd != -1)
        (void)close(shmfd);

    if (*shm_name)
        (void)shm_unlink(shm_name);

    if (smphr)
        (void)sem_close(smphr);

    if (*sem_name)
        (void)sem_unlink(sem_name);

    status = status != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return status;
print:
    print_usage(prog_name);
    return status;
}

void incr_subroutine(const char *sem_name, const char *shm_name, size_t memseg_size)
{
    void *memseg = NULL;
    int *incr_val = NULL, shmfd = -1;
    sem_t *smphr = NULL;
    int status = EXIT_SUCCESS;
    pid_t pid;

    pid = getpid();

    if ((shmfd = shm_open(shm_name, O_RDWR, 0 /*PERMS*/)) < 0) { // mode arg is ignored if shm_open() without O_CREAT
        handle_error("shm_open", errno);
        status = EXIT_FAILURE;
        goto end;        
    }

    if ((memseg = mmap(NULL, memseg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0)) == (void *)-1) {
        handle_error("mmap", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    if (shmfd != -1)
        (void)close(shmfd);
    shmfd = -1;

    if ((smphr = sem_open(sem_name, O_RDWR)) == SEM_FAILED) {
        handle_error("sem_open", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    incr_val = (int *)memseg;

    while (true) {

        if (sem_wait(smphr) < 0) {
            handle_error("sem_wait", errno);
            status = EXIT_FAILURE;
            goto end;
        }

        /* CRITICAL SECTION START: BUSY STATE */
        if (incr_val[INCR_IDX] < incr_val[CAP_IDX]) {
            incr_val[INCR_IDX]++;
            printf("PID%d incremented. Val=%d\n", pid, incr_val[INCR_IDX]);
        }

        if (incr_val[INCR_IDX] >= incr_val[CAP_IDX]) {
            printf("PID%d reached %d\n", pid, incr_val[CAP_IDX]);
            if (sem_post(smphr) < 0) {
                handle_error("sem_post", errno);
                status = EXIT_FAILURE;
                goto end;
            }
            break;
        }

        /* CRITICAL SECTION END: AVAILABLE STATE */
        if (sem_post(smphr) < 0) {
            handle_error("sem_post", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        sleep(1);
    }

end:
    if (memseg && munmap(memseg, memseg_size) < 0) {
        handle_error("munmap", errno);
        status = EXIT_FAILURE;
    }

    if (shmfd != -1)
        (void)close(shmfd);

    if (smphr)
        (void)sem_close(smphr);

    printf("%d is exiting. status = %d\n", pid, status);
    exit(status);
}

void print_usage(const char *prog_name)
{
    printf("Usage:\n");
    printf("1) %s -%c\n", prog_name, HELP_OPT);
    printf("2) %s -%c %s\n", prog_name, FILE_NAME_OPT, EXAMPLE_FILENAME);
    printf("3) %s -%c %s -%c 4 -%c 4\n", prog_name, FILE_NAME_OPT, EXAMPLE_FILENAME, INCR_OPT, PROC_OPT);
}

bool options_bundled(int opt, const char *optarg)
{
    bool bundled = false;

    if (opt <= 0 || !optarg)
        return false;

    if (*optarg == '-') {
        handle_error_msg("error: bundled options, i.e \"-%c%c\" are not allowed\n", opt, *optarg);
        bundled = true;
    }

    return bundled;
}
