#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <limits.h>
#define PROG_NAME "shm_server"
#define DEFAULT_PATHNAME "/proc/meminfo"
#define PATHNAME "./feedlot/message"
#define EXAMPLE_PATH "/path/to/file"
#define MAGIC_ID 'A'
#define SHMEM_FILE_OPT 'm'
#define HELP_OPT 'h'
#define MEMSIZE_OPT 's'
#define BUF_SIZ 256
#define DEFAULT_MEMSEGSIZE 4096
#define PERMS 0666 // -rw-rw-rw
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

void print_usage(void);
bool options_bundled(int opt, const char *optarg);

int main(int argc, char **argv)
{
    const char *shmem_file = NULL;
    char *payload = NULL;
    void *memseg = NULL;
    long size = -1;
    size_t memseg_size = 0;
    size_t memseg_size_max = 0;
    struct shmid_ds buf = {0};
    struct stat sb = {0};
    int page_size = -1;
    int opt = 0, ret = EXIT_SUCCESS;
    int shmid = -1, key = -1;

    if ((page_size = getpagesize()) < 0) {
        handle_error("getpagesize", errno);
        ret = EXIT_FAILURE;
        goto end;
    }
    printf("page_size = %d\n", page_size);
    /* since SHMMAX is ungodly huge, we will cap the max payload size to PAGE_SIZE times 4 */
    memseg_size_max = (size_t)page_size * 4;
    printf("Max memory segment size = %zu\n", memseg_size_max);
    /* : at the beginning suppresses default error message from getopt() */
    while ((opt = getopt(argc, argv, ":hm:s:")) != -1) {
        switch (opt) {
            case 'm':
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                shmem_file = optarg;
                printf("shmem_file = %s\n", shmem_file);
                break;
            case 's':
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                errno = 0;
                size = strtol(optarg, NULL, 10);
                if (errno != 0) {
                    handle_error("strtol", errno);
                    ret = EXIT_FAILURE;
                    goto print;
                } else if (size <= 0) {
                    handle_error_msg("Selected memory segment size is %ld. -%c must be follwed by a strictly positive integer\n", size, MEMSIZE_OPT);
                    goto print;
                }
                memseg_size = (size_t)size;
                printf("Selected memory segment size = %zu\n", memseg_size);
                if (memseg_size > memseg_size_max) {
                    handle_error_msg("error: memory segment max is %zu, but your size is %zu\n", memseg_size_max, memseg_size);
                    goto print;
                }
                break;
            case 'h':
                goto print;
            case ':':
                if (optopt == 'm')
                    handle_error_msg("-%c requires an argument, e.g. %s\n", optopt, DEFAULT_PATHNAME);
                else if (optopt == 's')
                    handle_error_msg("-%c requires an argument, e.g. 12\n", optopt);
                goto print;
            default: // '?'
                handle_error_msg("-%c is an unknown option\n", optopt);
                goto print;
        }
    }

    if (!shmem_file) {
        printf("shmem_file is not specified\n");
        shmem_file = DEFAULT_PATHNAME;
        printf("Defaulting to '%s' to generate a key\n", shmem_file);
    }

    if (memseg_size <= 0) {
        printf("Memory segment size is not specifed\n");
        memseg_size = (size_t)page_size;
        printf("Defaulting to memory segment size = %zu\n", memseg_size);
    }

    if ((ret = stat(shmem_file, &sb)) < 0) {
        handle_error("stat", errno);
        goto end;        
    }

    if ((key = ftok(shmem_file, MAGIC_ID)) < 0) {
        handle_error("ftok", errno);
        ret = EXIT_FAILURE;
        goto end;
    }
    printf("Created SysV IPC key %d\n", key);

    if ((shmid = shmget(key, memseg_size + 1, IPC_CREAT | PERMS)) < 0) { // allocates shared memory returning SHMID - shared memory identifier
        handle_error("shmget", errno);
        ret = EXIT_FAILURE;
        goto end;
    }
    printf("Allocated SysV shared memory segment of %zu bytes. shmid=%d\n", memseg_size + 1, shmid);

    if ((memseg = shmat(shmid, NULL, 0)) == (void *)-1) { //attaches the base address of shared memory.
        handle_error("shmat", errno);
        ret = EXIT_FAILURE;
        goto end;        
    }
    printf("Attached shared memory segment. Addr=%p\n", (void *)memseg);

    memset(memseg, 0x0, memseg_size + 1); // in case our previous server failed but the memory segment was allocated. Clean out stale data
    payload = (char *)memseg;

    while (!*payload) {
        printf("Server waiting for changes in memseg...\n");
        sleep(1);
    }

    payload[memseg_size] = '\0'; // payload[memseg_size = 5] = '\0'
    printf("payload = %s\n", payload);

    /* detach memory segment from the calling process, but do not delete it: let the reader handle it */
    if ((ret = shmdt(memseg)) < 0) {
        handle_error("shmdt", errno);
        goto end;
    }
    printf("Detached shared memory segment\n");

    if ((ret = shmctl(shmid, IPC_STAT, &buf)) < 0) {
        handle_error("shmctl", errno);
        goto end;
    }
    printf("Shared memory stats:\n");
    printf("Key supplied to shmget(): %d\n", buf.shm_perm.__key);
    printf("Effective UID of owner: %d\n", buf.shm_perm.uid);
    printf("Effective GID of owner: %d\n", buf.shm_perm.gid);
    printf("Effective UID of creator: %d\n", buf.shm_perm.cuid);
    printf("Effective GID of creator: %d\n", buf.shm_perm.cgid);
    printf("Permissions +  SHM_DEST and SHM_LOCKED flags: %d\n", buf.shm_perm.mode);
    printf("Sequence number: %d\n", buf.shm_perm.__seq);
    printf("Size of segments (bytes): %ld\n", buf.shm_segsz);
    printf("Last attach time: %ld\n", buf.shm_atime);
    printf("Last detach time: %ld\n", buf.shm_dtime);
    printf("Last change time: %ld\n", buf.shm_ctime);
    printf("PID of creator: %d\n", buf.shm_cpid);
    printf("PID of last shmat()/shmdt(): %d\n", buf.shm_lpid);
    printf("Number of current attaches: %ld\n", buf.shm_nattch);

    /* when doing IPC_RMID, the third argument should be NULL */
    if ((ret = shmctl(shmid, IPC_RMID, NULL)) < 0) {
        handle_error("shmctl", errno);
        goto end;
    }
    printf("Deleted shared memory segment\n");
    /* If no critical error occurred, it's a success */
end:
    /* Make a failed state explicit if it exists */
    ret = ret != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    printf("ret = %d\n", ret);
    return ret;
print:
    print_usage();
    printf("ret = %d\n", ret);
    return ret;
}

void print_usage(void)
{
    printf("Usage:\n");
    printf("1) %s -%c\n", PROG_NAME, HELP_OPT);
    printf("2) %s -%c 4096\n", PROG_NAME, MEMSIZE_OPT);
    printf("3) %s -%c %s -%c 4096\n", PROG_NAME, SHMEM_FILE_OPT, DEFAULT_PATHNAME, MEMSIZE_OPT);
}

bool options_bundled(int opt, const char *optarg)
{
    bool bundled = false;

    if (opt <= 0 || !optarg)
        return false;

    /* -m -s are considered bundled options */
    if (*optarg == '-') {
        handle_error_msg("error: bundled options, i.e \"-%c%c\" are not allowed\n", opt, *optarg);
        bundled = true;
    }

    return bundled;
}