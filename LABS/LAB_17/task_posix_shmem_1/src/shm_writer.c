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
#define PROG_NAME "shm_writer"
#define DEFAULT_NAME "/shm_object"
#define EXAMPLE_PATH "/path/to/file"
#define FILE_OPT 'o'
#define NAME_OPT 'n'
#define HELP_OPT 'h'
#define BUF_SIZ 256
#define SHMMIN 1
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
    const char *file = NULL, *name = NULL;
    void *memseg = NULL;
    size_t payload_size = 0, payload_size_max = 0;
    ssize_t nbytes = -1;
    struct stat sb = {0};
    int opt = 0, fd = -1, shmfd = -1, page_size = -1, ret = EXIT_SUCCESS;

    while ((opt = getopt(argc, argv, ":h:n:o:")) != -1) { // : at the beginning suppresses default error message from getopt()
        switch (opt) {
            case 'n':
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                name = optarg;
                printf("shm_name = %s\n", name);
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
                else if (optopt == 'n')
                    handle_error_msg("-%c requires an argument, e.g. %s\n", optopt, DEFAULT_NAME);
                goto print;
            default: // '?'
                handle_error_msg("-%c is an unknown option\n", optopt);
                goto print;
        }
    }

    if (!file) {
        handle_error_msg("error: file is not specified. Perhaps -%c is missing?\n", FILE_OPT);
        goto print;
    } else if (!name) {
        name = DEFAULT_NAME;
        printf("Name is not specified. Defaulting to '%s'\n", name);
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

    payload_size_max = (size_t)page_size * 4; // since SHMMAX is ungodly huge, we will cap the max payload size to PAGE_SIZE times 4 */
    if (payload_size < (size_t)SHMMIN) {
        handle_error_msg("error: SHMMIN is %d, but payload_size is %zu\n", SHMMIN, payload_size);
        ret = EXIT_FAILURE;
        goto end;
    } else if (payload_size > payload_size_max) {
        handle_error_msg("error: payload max is %zu, but payload_size is %zu\n", payload_size_max, payload_size);
        ret = EXIT_FAILURE;
        goto end;
    }

    if (!(payload = calloc(1, payload_size + 1))) { // null-terminating byte as a safety check
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

    (void)close(fd), fd = -1; // Close the file immediately
    /* shm_open acts like open(). It creates or opens a named object in the OS filesystem (usually under /dev/shm on Linux). */
    /* When you create a new POSIX shared memory object using shm_open, it starts with a length of 0 bytes. */
    if ((shmfd = shm_open(name, O_CREAT | O_RDWR, PERMS)) < 0) {
        handle_error("shm_open", errno);
        ret = EXIT_FAILURE;
        goto end;
    }
    /* ftruncate sets the shared memory object's ("file") size to nbytes, just like allocating disk space */
    if ((ret = ftruncate(shmfd, nbytes + 1)) < 0) {
        handle_error("ftruncate", errno);
        goto end;
    }
    /* When you call mmap, you are asking the Kernel to edit your process's Page Table.
      The Kernel verifies that you have the correct permissions, finds an empty slot in your virtual address space, and writes the new mapping entry into your table */
    /* The Kernel goes into Process A's Page Table and maps virtual address 0x5000 to physical RAM address 0x9999.
       Then, it goes into Process B's Page Table and maps virtual address 0x7000 to that exact same physical RAM address 0x9999 */
    if ((memseg = mmap(NULL, nbytes + 1, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0)) == (void *)-1) {
        handle_error("mmap", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    (void)close(shmfd), shmfd = -1;
    /* Instead of using read() and write() system calls to move data back and forth, mmap assigns a regular pointer (like char* ptr) to that "file." */
    /* mmap maps nbytes + 1 of file to physical RAM, returns the virtual address of the mapped memory */
    (void)memcpy(memseg, payload, nbytes);
    printf("Copied payload to %p\n", (void *)memseg);

    if ((ret = munmap(memseg, nbytes + 1)) < 0) { // unmaps nbytes + 1 of file from memory by deleting a Page Table entry
        handle_error("munmap", errno);
        goto end;
    }
end:
    if (payload) free(payload);
    if (fd != -1) (void)close(fd);
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
    printf("1) %s -%c\n", PROG_NAME, HELP_OPT);
    printf("2) %s -%c %s\n", PROG_NAME, FILE_OPT, EXAMPLE_PATH);
    printf("3) %s -%c %s -%c %s\n", PROG_NAME, NAME_OPT, DEFAULT_NAME, FILE_OPT, EXAMPLE_PATH);
}

bool options_bundled(int opt, const char *optarg)
{
    bool bundled = false;

    if (opt <= 0 || !optarg)
        return bundled;

    if (*optarg == '-') {
        handle_error_msg("error: bundled options, i.e \"-%c%c\" are not allowed\n", opt, *(optarg + 1));
        bundled = true;
    }
    return bundled;
}
