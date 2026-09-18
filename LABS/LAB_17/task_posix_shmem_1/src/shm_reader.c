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
#define PROG_NAME "shm_reader"
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
   size_t memseg_size = 0;
   int opt = 0, shmfd = -1, ret = EXIT_SUCCESS;

   while ((opt = getopt(argc, argv, ":hn:")) != -1) { // : at the beginning suppresses default error message from getopt()
      switch (opt) {
         case 'n':
            if (!optarg || true == options_bundled(opt, optarg))
               goto print;
            name = optarg;
            printf("shm_name = %s\n", name);
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

   if ((shmfd = shm_open(name, O_RDONLY, PERMS)) < 0) {
      handle_error("shm_open", errno);
      ret = EXIT_FAILURE;
      goto end;
   }

   /* Using ftruncate in the reader program will cause "bus error" since using shm_open() with flag "O_RDONLY"
      Implies the memory object is already present */
#if 0
      if ((ret = ftruncate(shmfd, SEG_SIZE)) < 0) {
         handle_error("ftruncate", errno);
         ret = EXIT_FAILURE;
         goto end;
      }
#endif

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

   memseg_size = (size_t)sb.st_size;
   /* shared read-only memory area */
   if ((memseg = mmap(NULL, memseg_size, PROT_READ, MAP_SHARED, shmfd, 0)) == (void *)-1) {
      handle_error("mmap", errno);
      ret = EXIT_FAILURE;
      goto end;
   }
   printf("Attached shared memory of %zu bytes long\n", memseg_size);

   (void)close(shmfd), shmfd = -1;

   payload = memseg;
   printf("Payload: %s\n", payload);
   /* Remove memory mapping from the calling process */
   if ((ret = munmap(memseg, memseg_size)) < 0) {
      handle_error("munmap", errno);
      goto end;
   }
   /* Remove POSIX memory object if the reference count is 0 */
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
   printf("3) %s -%c %s\n", PROG_NAME, NAME_OPT, DEFAULT_NAME);
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
