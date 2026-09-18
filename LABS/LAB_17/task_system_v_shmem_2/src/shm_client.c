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
#define PROG_NAME "shm_client"
#define DEFAULT_PATHNAME "/proc/meminfo"
#define PATHNAME "./feedlot/message"
#define EXAMPLE_PATH "/path/to/file"
#define MAGIC_ID 'A'
#define SHMEM_FILE_OPT 'm'
#define HELP_OPT 'h'
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
    size_t payload_size = 0;
    size_t len = 0;
    struct shmid_ds buf = {0};
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

    /* Try to get access to shmem */
    if ((shmid = shmget(key, 0, 0)) < 0) {
        handle_error("shmget", errno);
        ret = EXIT_FAILURE;
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
    if (!(payload = calloc(1, payload_size))) {
        handle_error("calloc", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    printf("Enter message: ");
    /* if payload_size is 6, the sixth byte is reserved for null-terminator */
    if (!fgets(payload, payload_size, stdin)) { // fgets(buf, n, stdin) reads at most n - 1 bytes
        handle_error("fgets", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    if (!*payload || *payload == '\n') {
        printf("No message\n");
        goto end;
    }

    /* len is guaranteed to be >= 1 because of payload checks above */
    len = strlen(payload);
    if (payload[len - 1] == '\n') {
        payload[len - 1] = '\0'; // remove newline character
        len--;
    } else {
        int c;
        printf("Input truncated. Read leftover data\n");
        while ((c = getchar()) != '\n' && c != EOF);
    }

    if ((memseg = shmat(shmid, NULL, 0)) == (void *)-1) {
        handle_error("shmat", errno);
        ret = EXIT_FAILURE;
        goto end; 
    }
    printf("Attached shared memory segment. Addr=%p\n", (void *)memseg);

    (void)memcpy(memseg, payload, len + 1); // null-terminator, too
    printf("payload = %s\n", payload);

    if ((ret = shmdt(memseg)) < 0) {
        handle_error("shmdt", errno);
        goto end;        
    }
    printf("Detached shared memory segment\n");
    /* If no critical error occurred, it's a success */
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
    printf("1) %s\n", PROG_NAME);
    printf("2) %s -%c\n", PROG_NAME, HELP_OPT);
    printf("3) %s -%c %s\n", PROG_NAME, SHMEM_FILE_OPT, DEFAULT_PATHNAME);
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