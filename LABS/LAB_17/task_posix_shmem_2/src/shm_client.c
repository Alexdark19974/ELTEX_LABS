#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <limits.h>
#define PROG_NAME "shm_client"
#define DEFAULT_NAME "/shm_object"
#define NAME_OPT 'n'
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
    const char *name = NULL;
    void *memseg = NULL;
    struct stat sb = {0};
    size_t memseg_size = 0, payload_size = 0, len = 0;
    int opt = 0, shmfd = -1, ret = EXIT_SUCCESS;

    /* : at the beginning suppresses default error message from getopt() */
    while ((opt = getopt(argc, argv, ":hn:")) != -1) {
        switch (opt) {
            case 'n':
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                name = optarg;
                printf("name = %s\n", name);
                break;
            case 'h':
                goto print;
            case ':':
                if (optopt == 'n')
                    handle_error_msg("-%c requires an argument, e.g. %s\n", optopt, DEFAULT_NAME);
                goto print;
            default: // '?'
                handle_error_msg("-%c is an unknown option\n", optopt);
                goto print;
        }
    }

    if (!name) {
        name = DEFAULT_NAME;
        printf("Name is not specified. Defaulting to '%s'\n", name);
    }

    if ((shmfd = shm_open(name, O_RDWR, PERMS)) < 0) { // O_WRONLY doesn't work here
        handle_error("shm_open", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    /* it's more appropriate to use fstat() than stat() because POSIX shared memory may not be backed by a real file */
    if ((ret = fstat(shmfd, &sb)) < 0) {
        handle_error("stat", errno);
        goto end;
    }

    if (((sb.st_mode & S_IFMT) != S_IFREG)) {
        switch (sb.st_mode & S_IFMT) {
            case S_IFDIR:
                handle_error_msg("error: %s is a directory\n", name);
                break;
            case S_IFIFO:
                handle_error_msg("error: %s is a FIFO/pipe\n", name);
                break;
            case S_IFBLK:
                handle_error_msg("error: %s is a block device\n", name);
                break;
            case S_IFSOCK:
                handle_error_msg("error: %s is a socket\n", name);
                break;
            case S_IFCHR:
                handle_error_msg("error: %s is a character device\n", name);
                break;
            case S_IFLNK:
                handle_error_msg("error: %s is a symlink\n", name);
                break;
            default:
                handle_error_msg("error: %s is unknown\n", name);
                break;
        }
        goto end;
    }

    payload_size = (size_t)sb.st_size;
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

    memseg_size = (size_t)sb.st_size;
    if ((memseg = mmap(NULL, memseg_size, PROT_WRITE, MAP_SHARED, shmfd, 0)) == (void *)-1) {
        handle_error("mmap", errno);
        ret = EXIT_FAILURE;
        goto end;
    }
    printf("Attached shared memory of %zu bytes long\n", memseg_size);

    (void)close(shmfd), shmfd = -1;

    (void)memcpy(memseg, payload, len + 1); // null-terminator, too
    printf("payload = %s\n", payload);

    if ((ret = munmap(memseg, memseg_size)) < 0) {
        handle_error("munmap", errno);
        ret = EXIT_FAILURE;
        goto end;
    }
end:
    if (payload) free(payload);
    if (shmfd != -1) (void)close(shmfd);
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
    printf("3) %s -%c %s\n", PROG_NAME, NAME_OPT, DEFAULT_NAME);
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