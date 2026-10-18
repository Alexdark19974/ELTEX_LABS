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
#define PROG_NAME "shm_writer"
#define DEFAULT_PATHNAME "/proc/meminfo"
#define PATHNAME "./feedlot/message"
#define EXAMPLE_PATH "/path/to/file"
#define MAGIC_ID 'A'
#define FILE_OPT 'o'
#define SHMEM_OPT 'm'
#define HELP_OPT 'h'
#define BUF_SIZ 256
#ifndef SHMMIN
#define SHMMIN 1
#endif
#define PATHNAME_SIZE 2048 // is PATHNAMEMAX is usually 8192
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
    const char *file = NULL;
    const char *shmem_file = NULL;
    void *memseg = NULL;
    size_t payload_size = 0;
    size_t payload_size_max = 0;
    ssize_t nbytes = -1;
    struct stat sb = {0};
    int page_size = -1;
    int opt = 0, fd = -1, ret = EXIT_SUCCESS;
    int shmid = -1, key = -1;

    /* : at the beginning suppresses default error message from getopt() */
    while ((opt = getopt(argc, argv, ":hm:o:")) != -1) {
        switch (opt) {
            case 'm':
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                shmem_file = optarg;
                printf("shmem_file = %s\n", shmem_file);
                break;
            case 'o':
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                file = optarg;
                printf("file = %s\n", file);
                break;
            case 'h':
                goto print;
            case ':':
                if (optopt == 'o')
                    handle_error_msg("-%c requires an argument, e.g. %s\n", optopt, EXAMPLE_PATH);
                else if (optopt == 'm')
                    handle_error_msg("-%c requires an argument, e.g. %s\n", optopt, DEFAULT_PATHNAME);
                goto print;
            default: // '?'
                handle_error_msg("-%c is an unknown option\n", optopt);
                goto print;
        }
    }

    if (!file) {
        handle_error_msg("error: file is not specified. Perhaps -%c is missing?\n", FILE_OPT);
        goto print;
    } else if (!shmem_file) {
        printf("shmem_file is not specified\n");
        shmem_file = DEFAULT_PATHNAME;
        printf("Defaulting to '%s' to generate a key\n", shmem_file);
    }

    if ((ret = stat(shmem_file, &sb)) < 0) {
        handle_error("stat", errno);
        goto end;        
    }

    if ((ret = stat(file, &sb)) < 0) {
        handle_error("stat", errno);
        goto end;        
    }

    if (((sb.st_mode & S_IFMT) != S_IFREG)) {
        switch (sb.st_mode & S_IFMT) {
            case S_IFDIR:
                handle_error_msg("error: %s is a directory\n", file);
                break;
            case S_IFIFO:
                handle_error_msg("error: %s is a FIFO/pipe\n", file);
                break;
            case S_IFBLK:
                handle_error_msg("error: %s is a block device\n", file);
                break;
            case S_IFSOCK:
                handle_error_msg("error: %s is a socket\n", file);
                break;
            case S_IFCHR:
                handle_error_msg("error: %s is a character device\n", file);
                break;
            case S_IFLNK:
                handle_error_msg("error: %s is a symlink\n", file);
                break;
            default:
                handle_error_msg("error: %s is unknown\n", file);
                break;
        }
        ret = EXIT_SUCCESS; // not really an error, just a different file type
        goto end;
    }

    payload_size = sb.st_size;
    printf("payload_size = %zu\n", payload_size);

    if ((page_size = getpagesize()) < 0) {
        handle_error("getpagesize", errno);
        ret = EXIT_FAILURE;
        goto end;
    }
    printf("page_size = %d\n", page_size);

    /* since SHMMAX is ungodly huge, we will cap the max payload size to PAGE_SIZE times 4 */
    payload_size_max = (size_t)page_size * 4;

    if (payload_size < (size_t)SHMMIN) {
        handle_error_msg("error: SHMMIN is %d, but payload_size is %zu\n", SHMMIN, payload_size);
        ret = EXIT_FAILURE;
        goto end;
    } else if (payload_size > payload_size_max) {
        handle_error_msg("error: payload max is %zu, but payload_size is %zu\n", payload_size_max, payload_size);
        ret = EXIT_FAILURE;
        goto end;
    }

    /* null-terminating byte as a safety check */
    if (!(payload = calloc(1, payload_size + 1))) {
        handle_error("calloc", errno);
        ret = EXIT_FAILURE;
        goto end;        
    }    

    printf("file to read = %s\n", file);
    if ((fd = open(file, O_RDONLY)) < 0) {
        handle_error("open", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    if ((nbytes = read(fd, payload, payload_size)) < 0) {
        ret = EXIT_FAILURE;
        handle_error("read", errno);
        goto end;
    }
    printf("Read %ld bytes\n", nbytes);

    if ((key = ftok(shmem_file, MAGIC_ID)) < 0) {
        handle_error("ftok", errno);
        ret = EXIT_FAILURE;
        goto end;
    }
    printf("Created SysV IPC key %d\n", key);

    if ((shmid = shmget(key, payload_size + 1, IPC_CREAT | PERMS)) < 0) { // allocates shared memory returning SHMID - shared memory identifier
        handle_error("shmget", errno);
        ret = EXIT_FAILURE;
        goto end;
    }
    printf("Allocated SysV shared memory segment of %zu bytes. shmid=%d\n", payload_size + 1, shmid);

    if ((memseg = shmat(shmid, NULL, 0)) == (void *)-1) { //attaches the base address of shared memory.
        handle_error("shmat", errno);
        ret = EXIT_FAILURE;
        goto end;        
    }
    printf("Attached shared memory segment. Addr=%p\n", (void *)memseg);

    (void)memcpy(memseg, payload, nbytes + 1); // null-terminator included
    printf("Copied payload to %p\n", (void *)memseg);

    /* detach memory segment from the calling process, but do not delete it: let the reader handle it */
    if ((ret = shmdt(memseg)) < 0) {
        handle_error("shmdt", errno);
        goto end;
    }
    printf("Detached shared memory segment\n");
end:
    if (payload)
        free(payload);
    if (fd != -1)
        (void)close(fd);
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
    printf("2) %s -%c %s\n", PROG_NAME, FILE_OPT, EXAMPLE_PATH);
    printf("3) %s -%c %s -%c %s\n", PROG_NAME, SHMEM_OPT, DEFAULT_PATHNAME, FILE_OPT, EXAMPLE_PATH);
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