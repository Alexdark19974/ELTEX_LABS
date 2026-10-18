// https://embeddedpathashala.com/sending-receiving-messages-free-linux-system-programming-course/ -> good source
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#define MESSAGE "./feedlot/message"
#define MESSAGE_2 "./feedlot/message2"
#define MAGIC_ID 'A'
#define BUF_SIZ 256
#define MSG_SIZE 2048 // is MSGMAX is usually 8192
#define PERMS 0666 // -rw-rw-rw
#define MSG_TYPE 1 // can be consedered a priority field
#define MSG_NUM 2
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)
#define handle_error_msg(fmt, ...) do { \
                            fprintf(stderr, fmt, __VA_ARGS__); \
                        } while(0)

typedef struct {
    long mtype;
    char mtext[MSG_SIZE + 1];
} Msg;

// #define PAYLOAD_SIZE (sizeof(Msg) - sizeof(long)) // incorrect due to alignment issues

int main(void)
{
    char *msgs[MSG_NUM];
    Msg msg = {0, {0}};
    ssize_t nbytes = 0;
    int ret = EXIT_SUCCESS;
    int fd = -1;
    int quid = -1;
    key_t key = -1;
    int send_num = 0;
    int i;

    for (i = 0; i < MSG_NUM; i++) {
        msgs[i] = i == 0 ? MESSAGE : MESSAGE_2;

        if ((fd = open(msgs[i], O_RDONLY)) < 0) {
            handle_error("open", errno);
            ret = EXIT_FAILURE;
            goto fail;
        }

        /* read a text file in 1 chunk */
        memset((void *)msg.mtext, 0, sizeof(msg.mtext));
        if ((nbytes = read(fd, msg.mtext, sizeof(msg.mtext) - 1)) < 0) {
            handle_error("read", errno);
            ret = EXIT_FAILURE;
            goto fail;
        }

        printf("read %ld bytes from %s\n", nbytes, msgs[i]);

        msg.mtype = i + 1;

        /* Enter once */
        if (key == -1) {
            /* convert a pathname and a project identifier to a System V IPC key */
            if ((key = ftok(MESSAGE, MAGIC_ID)) < 0) {
                handle_error("ftok", errno);
                ret = EXIT_FAILURE;
                goto fail;
            }
        }

        /* Enter once */
        if (quid == -1) {
            /* if a queue is created first by a receiver, msgget() will return quid of the existing queue */
            if ((quid = msgget(key, PERMS | IPC_CREAT)) < 0) {
                handle_error("msgget", errno);
                ret = EXIT_FAILURE;
                goto fail;
            }
            printf("msg queue num = %d\n", quid);
        }

        /* instead of sending 2048 bytes, send the number of bytes read */
        if ((msgsnd(quid, (void *)&msg, nbytes, 0)) < 0) { // if msgflg is 0, then msgsnd will bock if the queue is full
            handle_error("msgsnd", errno);
            handle_error_msg("Failed to send message from %s\n", MESSAGE);
            ret = EXIT_FAILURE;
        } else {
            printf("Sent message, pri = %ld\n", msg.mtype);
            send_num++;
        }

        (void)close(fd);
        fd = -1;
    }
    printf("Sent messages in total = %d\n", send_num);
fail:
    if (fd != -1)
        (void)close(fd);
    /* Make a failed state explicit if it exists */
    ret = ret != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return ret;
}
