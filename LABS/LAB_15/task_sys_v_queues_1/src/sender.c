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
#define MAGIC_ID 'A'
#define BUF_SIZ 256
#define MSG_SIZE 2048 // is MSGMAX is usually 8192
#define PERMS 0666 // -rw-rw-rw
#define MSG_TYPE 1 // can be consedered a priority field
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
    ssize_t nbytes = -1;
    Msg msg = {0, {0}};
    int fd = -1;
    int quid;
    int ret = EXIT_SUCCESS;
    key_t key;

    if ((fd = open(MESSAGE, O_RDONLY)) < 0) {
        handle_error("open", errno);
        ret = EXIT_FAILURE;
        goto fail;
    }

    /* read a text file in 1 chunk */
    if ((nbytes = read(fd, msg.mtext, MSG_SIZE)) < 0) {
        handle_error("read", errno);
        ret = EXIT_FAILURE;
        goto fail;
    }

    /* read() does not null-terminate, only reads raw data */
    /* if we read a file that consists of plain text, then we need to null-terminate it on our own */
    // msg.mtext[MSG_SIZE - 1] = '\0'; // not needed with MSG_SIZE + 1

    printf("read %ld bytes from %s\n", nbytes, MESSAGE);

    msg.mtype = MSG_TYPE; // any (strictly) positive value; used by msgrcv to read data from queue of different priority
    
    /* convert a pathname and a project identifier to a System V IPC key */
    if ((key = ftok(MESSAGE, MAGIC_ID)) < 0) {
        handle_error("ftok", errno);
        ret = EXIT_FAILURE;
        goto fail;
    }

    /* if a queue is created first by a receiver, msgget() will return quid of the existing queue */
    if ((quid = msgget(key, PERMS | IPC_CREAT)) < 0) {
        handle_error("msgget", errno);
        ret = EXIT_FAILURE;
        goto fail;
    }

    /* instead of sending 2048 bytes, send the number of bytes read */
    if ((ret = msgsnd(quid, (void *)&msg, /* PAYLOAD_SIZE */ nbytes /* sizeof(msg.mtext) - 1*/, 0)) < 0) { // if msgflg is 0, then msgsnd will bock if the queue is full
        handle_error("msgsnd", errno);
        handle_error_msg("Failed to send message from %s\n", MESSAGE);
        // (void)msgctl(quid, IPC_RMID, NULL); // deleting the queue is a receiver's responsibility
        goto fail;
    }

    printf("Sent message\n");
    /* Explicitly set ret to EXIT_SUCCESS because msgsnd() stores its return value in ret (even though it is 0 on success) */
fail:
    if (fd != -1)
        (void)close(fd);
    /* Make a failed state explicit if it exists */
    ret = ret != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return ret;
}
