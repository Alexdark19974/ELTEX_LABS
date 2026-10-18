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
#define PROG_NAME "shm_reader"
#define DEFAULT_PATHNAME "/proc/meminfo"
#define MAGIC_ID 'A'
#define SHMEM_OPT 'm'
#define HELP_OPT 'h'
#define BUF_SIZ 256
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
    char *payload = NULL;
    const char *shmem_file = NULL;
    void *memseg = NULL;
    struct shmid_ds buf = {0};
    size_t payload_size = 0;
    struct stat sb = {0};
    int opt = 0, ret = EXIT_SUCCESS;
    int shmid = -1, key = -1;

    /* : at the beginning suppresses default error message from getopt() */
    while ((opt = getopt(argc, argv, ":hm:")) != -1) {
        switch (opt) {
            case 'm':
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                shmem_file = optarg;
                printf("shmem_file = %s\n", shmem_file);
                break;
            case 'h':
                goto print;
            case ':':
                if (optopt == 'm')
                    handle_error_msg("-%c requires an argument, e.g. %s\n", optopt, DEFAULT_PATHNAME);
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

    /* We do not create shared memory, only get access to it if possible */
    if ((shmid = shmget(key, 0, 0)) < 0) {
        handle_error("shmget", errno);
        goto end;
    }
    printf("Obtained SysV shared memory segment. shmid=%d\n", shmid);

    if ((ret = shmctl(shmid, IPC_STAT, &buf)) < 0) {
        handle_error("shmctl", errno);
        goto end;
    }

    printf("attaches: %ld\n", buf.shm_nattch);
    printf("shmem segment size: %zu\n", buf.shm_segsz);

    payload_size = buf.shm_segsz;
    printf("payload_size = %zu\n", payload_size);

    /* null-terminating byte as a safety check */
    if (!(payload = calloc(1, payload_size + 1))) {
        handle_error("calloc", errno);
        ret = EXIT_FAILURE;
        goto end;        
    }

    if ((memseg = shmat(shmid, NULL, SHM_RDONLY)) == (void *)-1) {
        handle_error("shmat", errno);
        ret = EXIT_FAILURE;
        goto end; 
    }
    printf("Attached read-only shared memory segment. Addr=%p\n", (void *)memseg);

    (void)memcpy(payload, memseg, payload_size); // payload_size includes a null-terminator thanks to the server's end
    payload[payload_size] = '\0'; // in case someone decides to replace calloc() with malloc() someday or the sever stops allocating an extra byte.
    printf("payload = %s\n", payload);

    if ((ret = shmdt(memseg)) < 0) {
        handle_error("shmdt", errno);
        goto end;        
    }
    printf("Detached shared memory segment\n");

    if ((ret = shmctl(shmid, IPC_RMID, NULL)) < 0) {
        handle_error("shmctl", errno);
        goto end;
    }
    printf("Deleted shared memory segment\n");
end:
    if (payload)
        free(payload);
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
    printf("2) %s -%c %s\n", PROG_NAME, SHMEM_OPT, DEFAULT_PATHNAME);
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