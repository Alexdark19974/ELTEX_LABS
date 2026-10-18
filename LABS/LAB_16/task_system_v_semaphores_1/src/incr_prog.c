#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#define DEFAULT_INCR_MAX_NUM 20
#define DEFAULT_PROC_NUM 2
#define DEFAULT_PATHNAME "/proc/meminfo"
#define MAGIC_ID 'A'
#define MAGIC_ID2 'B'
#define SHMEM_FILE_OPT 'm'
#define INCR_OPT 'i'
#define PROC_OPT 'p'
#define HELP_OPT 'h'
#define BUF_SIZ 256
#define PERMS 0666 // -rw-rw-rw
#define KEY_NUM 2
#define SEM_NUM 1
#define INCR_IDX 0
#define CAP_IDX 1
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
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};
void print_usage(const char *prog_name);
bool options_bundled(int opt, const char *optarg);
void incr_subroutine(int shmid, int semid);

int main(int argc, char **argv)
{
    const char *shmem_file = NULL, *prog_name = NULL, magic[KEY_NUM] = {MAGIC_ID, MAGIC_ID2};
    void *memseg = NULL;
    int *incr_val = NULL;
    size_t memseg_size = 0;
    long num = 0;
    struct stat sb = {0};
    union semun arg;
    int opt = 0, wret = 0, status = EXIT_SUCCESS;
    int shmid = -1, semid = -1, key[KEY_NUM] = {-1};
    int i = 0, incr_num = 0, proc_num = 0;
    pid_t *pid_list = NULL;

    prog_name = argv[0];

    while ((opt = getopt(argc, argv, ":hm:i:p:")) != -1) {
        switch (opt) {
            case SHMEM_FILE_OPT:
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                shmem_file = optarg;
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
                if (optopt == SHMEM_FILE_OPT)
                    handle_error_msg("-%c requires an argument, e.g. %s\n", optopt, DEFAULT_PATHNAME);
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

    if (!shmem_file) {
        handle_error_msg("file for shared memory is not specified! Please specify a real accessible file such as '%s'\n",
            DEFAULT_PATHNAME);
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

    printf("shared memory file = %s\n", shmem_file);
    if (stat(shmem_file, &sb) < 0) {
        handle_error("stat", errno);
        status = EXIT_FAILURE;
        goto end;        
    }

    for (i = 0; i < KEY_NUM; i++) {
        if ((key[i] = ftok(shmem_file, magic[i])) < 0) {
           handle_error("ftok", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        printf("Created SysV IPC key %d\n", key[i]);

    }
    i = 0;

    if (!(pid_list = calloc((size_t)proc_num, sizeof(pid_t)))) {
        handle_error("calloc", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    memseg_size = sizeof(int) + sizeof(int);
    if ((shmid = shmget(key[i], memseg_size, IPC_CREAT | IPC_EXCL | PERMS)) < 0) { // allocates shared memory returning SHMID - shared memory identifier
        handle_error("shmget", errno);
        status = EXIT_FAILURE;
        goto end;
    }
    i++;
    printf("Allocated SysV shared memory segment of %zu bytes. shmid=%d\n", memseg_size, shmid);

    /* Create a new semaphore set of semaphores of SEM_NUM */
    if ((semid = semget(key[i], SEM_NUM, IPC_CREAT | IPC_EXCL | PERMS)) < 0) {
        handle_error("semget", errno);
        status = EXIT_FAILURE;
        goto end;
    }
    printf("Created SysV IPC semaphore set. semid=%d, semnum = %d\n", semid, SEM_NUM);
    i = 0;

    if ((memseg = shmat(shmid, NULL, 0)) == (void *)-1) { //attaches the base address of shared memory.
        handle_error("shmat", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    incr_val = (int *)memseg;
    incr_val[INCR_IDX] = 0; // set it to 0 explicitly
    incr_val[CAP_IDX] = incr_num;

    arg.val = 1; // set semaphore semnum's semval to 1, make it available
    if (semctl(semid, 0, SETVAL, arg) < 0) {
        handle_error("semctl", errno);
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
            incr_subroutine(shmid, semid);
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

    if (memseg && shmdt(memseg) < 0) {
        handle_error("shmdt", errno);
        status = EXIT_FAILURE;
    }

    if (shmid != -1 && shmctl(shmid, IPC_RMID, NULL) < 0) {
        handle_error("shmctl", errno);
        status = EXIT_FAILURE;
    }

    if (semid != -1 && semctl(semid, 0, IPC_RMID, NULL) < 0) { // remove the semaphore set, semnum is ignored, so 0
        handle_error("semctl", errno);
        status = EXIT_FAILURE;
    }

    status = status != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return status;
print:
    print_usage(prog_name);
    return status;
}

void incr_subroutine(int shmid, int semid)
{
    void *memseg = NULL;
    int *incr_val = NULL;
    struct sembuf lock = {0, -1, SEM_UNDO}; // subtract 1 from semval; read upon what SEM_UNDO and semadj do and why they are essential
    struct sembuf unlock = {0, 1, SEM_UNDO}; // add 1 to semval
    int status = EXIT_SUCCESS;
    int pid;
    const int semops = 1;

    pid = getpid();

    if ((memseg = shmat(shmid, NULL, 0)) == (void *)-1) { //attaches the base address of shared memory.
        handle_error("shmat", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    incr_val = (int *)memseg;

    while (true) {
        /* subtracting 1 from semaphore's semval=1 -> semval=0 */
        /* if another process tries to decrement semval=0, it blocks on semop() */
        if (semop(semid, &lock, semops) < 0) { // semval is unsigned short, cannot be less than 0
            handle_error("semop", errno);
            status = EXIT_FAILURE;
            goto end;
        }

        /* CRITICAL SECTION START */
        if (incr_val[INCR_IDX] < incr_val[CAP_IDX]) {
            incr_val[INCR_IDX]++;
            printf("PID%d incremented. Val=%d\n", pid, incr_val[INCR_IDX]);
        }

        if (incr_val[INCR_IDX] >= incr_val[CAP_IDX]) {
            printf("PID%d reached %d\n", pid, incr_val[CAP_IDX]);
            if (semop(semid, &unlock, semops) < 0) {
                handle_error("semop", errno);
                status = EXIT_FAILURE;
                goto end;
            }
            break;
        }

        /* CRITICAL SECTION END: adding 1 to semaphore's semval. semval=0 -> semval=1 (AVAILABLE STATE) */
        if (semop(semid, &unlock, semops) < 0) {
            handle_error("semop", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        sleep(1);
    }
end:
    if (memseg && shmdt(memseg) < 0) {
        handle_error("shmdt", errno);
        status = EXIT_FAILURE;
    }
    printf("%d is exiting. status = %d\n", pid, status);
    exit(status);
}

void print_usage(const char *prog_name)
{
    printf("Usage:\n");
    printf("1) %s -%c\n", prog_name, HELP_OPT);
    printf("2) %s -%c %s -%c 4 -%c 4\n", prog_name, SHMEM_FILE_OPT, DEFAULT_PATHNAME, INCR_OPT, PROC_OPT);
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