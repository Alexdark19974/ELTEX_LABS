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
#define PROG_NAME "shm_server"
#define DEFAULT_NAME "/shm_object"
#define EXAMPLE_PATH "/path/to/file"
#define NAME_OPT 'n'
#define MEMSIZE_OPT 's'
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
    size_t memseg_size = 0, memseg_size_max = 0;
    long size = -1;
    int opt = 0, shmfd = -1, page_size = -1, ret = EXIT_SUCCESS;

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
    while ((opt = getopt(argc, argv, ":hn:s:")) != -1) {
        switch (opt) {
            case 'n':
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                name = optarg;
                printf("name = %s\n", name);
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
                if (optopt == 'n')
                    handle_error_msg("-%c requires an argument, e.g. %s\n", optopt, DEFAULT_NAME);
                else if (optopt == 's')
                    handle_error_msg("-%c requires an argument, e.g. 12\n", optopt);
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

    if (memseg_size <= 0) {
        printf("Memory segment size is not specifed\n");
        memseg_size = (size_t)page_size;
        printf("Defaulting to memory segment size = %zu\n", memseg_size);
    }

    if ((shmfd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, PERMS)) < 0) {
        handle_error("shm_open", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    if ((ret = ftruncate(shmfd, memseg_size + 1)) < 0) {
        handle_error("ftruncate", errno);
        goto end;
    }

    if ((memseg = mmap(NULL, memseg_size + 1, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0)) == (void *)-1) {
        handle_error("mmap", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    (void)close(shmfd), shmfd = -1;

    payload = (char *)memseg;

    while (!*payload) {
	    printf("Server waiting for changes in memseg...\n");
        sleep(1);
    }

    payload[memseg_size] = '\0'; // payload[memseg_size = 5] = '\0'
    printf("payload = %s\n", payload);

    if ((ret = munmap(memseg, memseg_size + 1)) < 0) { // unmaps nbytes + 1 of file from memory by deleting a Page Table entry
        handle_error("munmap", errno);
        goto end;
    }

	if ((ret = shm_unlink(name)) < 0) {
        handle_error("shm_unlink", errno);
        goto end;
	}
end:
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
    printf("2) %s -%c 4096\n", PROG_NAME, MEMSIZE_OPT);
    printf("3) %s -%c %s -%c 4096\n", PROG_NAME, NAME_OPT, DEFAULT_NAME, MEMSIZE_OPT);
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
