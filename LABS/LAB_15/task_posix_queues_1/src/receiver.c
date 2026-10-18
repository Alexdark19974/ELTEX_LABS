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
#define MSG_MAX_SIZE 8192 // standard MSG_MAX. can be changed by using mq_setattr()
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
    struct mq_attr attr = {0};
    unsigned int prio = PRI_NUM;
    mqd_t qfd = -1;
    int ret = EXIT_SUCCESS;

    attr.mq_maxmsg = MSG_MAX_NUM;
    attr.mq_msgsize = MSG_MAX_SIZE;
    /* Create or open a POSIX message queue. Signature is similar to open() */
    if ((qfd = mq_open(MSG_QUEUE, O_RDONLY | O_CREAT, PERMS, &attr)) < 0) {
        handle_error("mq_open", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    printf("queue num = %d\n", qfd);
    printf("priority is %d\n", prio);
    
    memset(&attr, 0x0, sizeof(attr));
    if (mq_getattr(qfd, &attr) < 0) {
        handle_error("mq_getattr", errno);
        ret = EXIT_FAILURE;
        goto end;
    }

    printf("messages in queue: %ld\n", attr.mq_curmsgs);
    if (attr.mq_curmsgs == 0) {
        printf("No messages. Leaving.\n");
        goto end;
    }

    payload_size = attr.mq_msgsize;

    printf("payload_size = %zu\n", payload_size);
    /* null-terminating byte as a safety check */
    if (!(payload = calloc(1, payload_size + 1))) {
        handle_error("calloc", errno);
        ret = EXIT_FAILURE;
        goto end;        
    }

    if ((nbytes = mq_receive(qfd, payload, payload_size, &prio)) < 0) {
        handle_error("mq_receive", errno);
        ret = EXIT_FAILURE;
        goto end;        
    }

    printf("Read %ld bytes from message queue\n", nbytes);
    printf("Message:\n");

    if (nbytes > 0)
        printf("%s\n", payload);
    else
        printf("No payload\n");
end:
    if (payload)
        free(payload);
    if (qfd != -1)
        (void)mq_close(qfd);
    if (nbytes >= 0 && attr.mq_curmsgs == 1)
        (void)mq_unlink(MSG_QUEUE);
    /* Make a failed state explicit if it exists */
    ret = ret != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return ret;
}
