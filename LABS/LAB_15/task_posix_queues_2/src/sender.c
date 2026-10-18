#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#define MSG "./feedlot/message"
#define MSG_2 "./feedlot/message2"
#define MSG_QUEUE "/message"
#define MSG_MAX_SIZE 8192 // standard MSG_MAX. CANNOT be changed by using mq_setattr() after a queue beging initialized
#define MSG_MAX_NUM 8
#define BUF_SIZ 256
#define PRI_NUM 7
#define MSG_NUM 2
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
    char *msg = NULL;
    char *payload = NULL;
    size_t payload_size = 0;
    ssize_t nbytes = -1;
    struct stat sb;
    struct mq_attr attr = {0};
    unsigned int prio;
    mqd_t qfd = -1;
    int fd = -1, i;
    int ret = EXIT_SUCCESS;
    int msg_sent = 0;

    attr.mq_maxmsg = MSG_MAX_NUM;
    attr.mq_msgsize = MSG_MAX_SIZE;
    /* Create or open a POSIX message queue. Signature is similar to open() */
    if ((qfd = mq_open(MSG_QUEUE, O_WRONLY | O_CREAT, PERMS, &attr)) < 0) {
        handle_error("mq_open", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    printf("queue num = %d\n", qfd);

    for (i = 0; i < MSG_NUM; i++) {

        msg = i == 0 ? MSG : MSG_2;

        memset(&sb, 0x0, sizeof(struct stat));
        if (stat(msg, &sb) < 0) {
            handle_error("stat", errno);
            ret = EXIT_FAILURE;
            continue; // keep iterating
        }

        if (((sb.st_mode & S_IFMT) != S_IFREG)) {
            switch (sb.st_mode & S_IFMT) {
                case S_IFDIR:
                    handle_error_msg("error: %s is a directory\n", msg);
                    break;
                case S_IFIFO:
                    handle_error_msg("error: %s is a FIFO/pipe\n", msg);
                    break;
                case S_IFBLK:
                    handle_error_msg("error: %s is a block device\n", msg);
                    break;
                case S_IFSOCK:
                    handle_error_msg("error: %s is a socket\n", msg);
                    break;
                case S_IFCHR:
                    handle_error_msg("error: %s is a character device\n", msg);
                    break;
                case S_IFLNK:
                    handle_error_msg("error: %s is a symlink\n", msg);
                    break;
                default:
                    handle_error_msg("error: %s is unknown\n", msg);
                    break;
            }
            // ret = EXIT_FAILURE; // not a system error; just a different file type
            continue;
        }

        if (sb.st_size > attr.mq_msgsize) {
            handle_error_msg("error: message is too big to send: %ld vs. max %ld\n", sb.st_size, attr.mq_msgsize);
            // ret = EXIT_FAILURE; // not a system error; just too big a file
            continue;
        }

        payload_size = sb.st_size;
        printf("payload_size = %zu\n", payload_size);
        /* null-terminating byte as a safety check */
        if (!(payload = calloc(1, payload_size + 1))) {
            handle_error("calloc", errno);
            ret = EXIT_FAILURE; // cricital sytem error: immediately terminate execution
            break;
        }

        printf("message to read = %s\n", msg);
        if ((fd = open(msg, O_RDONLY)) < 0) {
            handle_error("open", errno);
            ret = EXIT_FAILURE; // system error, set status but keep iterating
            goto next;
        }

        if ((nbytes = read(fd, payload, payload_size)) < 0) {
            handle_error("read", errno);
            ret = EXIT_FAILURE; // system error, set status but keep iterating
            goto next;
        }

        printf("Read %ld bytes\n", nbytes);

        if (nbytes != (ssize_t)payload_size) {
            printf("warning: discrepancy in payload_size(%zu) and bytes read from the file (%ld)\nHas the file been modified mid-run?\n",
                payload_size, nbytes);
            /* choose the relevant size even if it's less than we allocated */
            payload_size = (size_t)nbytes;
        }

        prio = i + 1;
        printf("priority is %u\n", prio);
        if ((mq_send(qfd, payload, payload_size, prio)) < 0) {
            handle_error("mq_send", errno);
            ret = EXIT_FAILURE;
        } else {
            printf("Sent message\n");
            msg_sent++;
        }
next:
        free(payload);
        payload = NULL;
        if (fd != -1) {
            (void)close(fd);
            fd = -1;
        }
        payload_size = 0;
    }

    printf("Messages sent: %d\n", msg_sent);
end:
/* Everything is done within the loop above */
#if 0
    if (payload)
        free(payload);
    if (fd != -1)
        (void)close(fd);
#endif
    if (qfd != -1)
        (void)mq_close(qfd);
    /* Make a failed state explicit if it exists */
    ret = ret != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return ret;
}
