#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#define MSG "./feedlot/message"
#define MSG_QUEUE "/message"
#define MSG_MAX_SIZE 8192 // standard MSG_MAX. CANNOT be changed by using mq_setattr() after a queue beging initialized
#define MSG_MAX_NUM 8
#define BUF_SIZ 256
#define PRI_NUM 7
#define PERMS 0666 // -rw-rw-rw
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)
#define handle_error_msg(fmt, ...) do { \
                            fprintf(stderr, fmt, __VA_ARGS__); \
                        } while(0)
int main(void)
{
    char *payload = NULL;
    size_t payload_size = 0;
    ssize_t nbytes = -1;
    struct stat sb = {0};
    struct mq_attr attr = {0};
    unsigned int prio = PRI_NUM;
    mqd_t qfd = -1;
    int fd = -1;
    int ret = EXIT_SUCCESS;

    attr.mq_maxmsg = MSG_MAX_NUM;
    attr.mq_msgsize = MSG_MAX_SIZE;
    /* Create or open a POSIX message queue. Signature is similar to open() */
    if ((qfd = mq_open(MSG_QUEUE, O_WRONLY | O_CREAT, PERMS, &attr)) < 0) {
        handle_error("mq_open", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    printf("queue num = %d\n", qfd);
    printf("priority is %d\n", prio);  

    if (stat(MSG, &sb) < 0) {
        handle_error("stat", errno);
        ret = EXIT_FAILURE;
        goto end;        
    }

    if (((sb.st_mode & S_IFMT) != S_IFREG)) {
        switch (sb.st_mode & S_IFMT) {
            case S_IFDIR:
                handle_error_msg("error: %s is a directory\n", MSG);
                break;
            case S_IFIFO:
                handle_error_msg("error: %s is a FIFO/pipe\n", MSG);
                break;
            case S_IFBLK:
                handle_error_msg("error: %s is a block device\n", MSG);
                break;
            case S_IFSOCK:
                handle_error_msg("error: %s is a socket\n", MSG);
                break;
            case S_IFCHR:
                handle_error_msg("error: %s is a character device\n", MSG);
                break;
            case S_IFLNK:
                handle_error_msg("error: %s is a symlink\n", MSG);
                break;
            default:
                handle_error_msg("error: %s is unknown\n", MSG);
                break;
        }
        goto end;
    }

    if (sb.st_size > attr.mq_msgsize) {
        handle_error_msg("error: message is too big to send: %ld vs. max %ld\n", sb.st_size, attr.mq_msgsize);
        ret = EXIT_FAILURE;
        goto end;
    }

    payload_size = sb.st_size;
    printf("payload_size = %zu\n", payload_size);
    /* null-terminating byte as a safety check */
    if (!(payload = calloc(1, payload_size + 1))) {
        handle_error("calloc", errno);
        ret = EXIT_FAILURE;
        goto end;        
    }

    printf("message to read = %s\n", MSG);
    if ((fd = open(MSG, O_RDONLY)) < 0) {
        handle_error("open", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    if ((nbytes = read(fd, payload, payload_size)) < 0) {
        handle_error("read", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    printf("Read %ld bytes\n", nbytes);

    if (nbytes != (ssize_t)payload_size) {
        printf("warning: discrepancy in payload_size(%zu) and bytes read from the file (%ld)\nHas the file been modified mid-run?\n",
            payload_size, nbytes);
        /* choose the relevant size even if it's less than we allocated */
        payload_size = nbytes;
    }

    if ((mq_send(qfd, payload, payload_size, prio)) < 0) {
        handle_error("mq_send", errno);
        ret = EXIT_FAILURE;
        goto end;
    }
    printf("Sent message\n");
end:
    if (payload)
        free(payload);
    if (fd != -1)
        (void)close(fd);
    if (qfd != -1)
        (void)mq_close(qfd);
    /* Make a failed state explicit if it exists */
    ret = ret != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return ret;
}
