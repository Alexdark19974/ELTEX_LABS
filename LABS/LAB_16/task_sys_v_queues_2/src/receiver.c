#include <stdio.h>
#include <stdlib.h>
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
#define MSG_SIZE 2048 // is MSGMAX
#define PERMS 0666 // -rw-rw-rw
#define MSG_NUM 2
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

int main(void)
{
    ssize_t nbytes = 0;
    Msg msg = {0, {0}};
    int quid = -1;
    int i;
    int error_num = 0;
    int read_num = 0;
    int ret = EXIT_SUCCESS;
    key_t key;

    /* convert a pathname and a project identifier to a System V IPC key */
    key = ftok(MESSAGE, MAGIC_ID);
    if (key < 0) {
        handle_error("ftok", errno);
        ret = EXIT_FAILURE;
        goto fail;
    }

     /* if a queue is created first by a receiver, msgget() will return quid of the existing queue */
    quid = msgget(key, PERMS | IPC_CREAT);
    if (quid < 0) {
        handle_error("msgget", errno);
        ret = EXIT_FAILURE;
        goto fail;
    }

    for (i = 0; i < MSG_NUM; i++) {
        // read message of type MSG_TYPE
        memset((void *)msg.mtext, 0, sizeof(msg.mtext));
        /* prioritize message with the lowest msgtype or equal to the absolute value of -(MSG_NUM) */
        nbytes = msgrcv(quid, (void *)&msg, sizeof(msg.mtext) - 1, /* i + 1 */-(MSG_NUM), IPC_NOWAIT);
        if (nbytes < 0) {
            error_num = errno; // we retain errno because it is highly volatile between function calls
            if (error_num != ENOMSG) {
                handle_error("msgrcv", error_num);
                handle_error_msg("Failed to read message from %s\n", MESSAGE);
                ret = EXIT_FAILURE;
                goto fail;
            /* ENOMSG is not a critical error, it's just there are no message is all */
            } else
                handle_error_msg("No messages in queue%d\n", quid);
        } else {
            printf("Read %ld bytes\n", nbytes);
            printf("Message:\n");
            printf("%s\n", msg.mtext);
            read_num++;
        }
        error_num = 0; // erase stale ENOMSG if any
    }
    printf("Total mesasges read: %d\n", read_num);
fail:
    if (quid != -1) {
        /* we read something or payload is zero OR if it's an error other than ENOMSG, we forcefully remove the message queue */
        if (read_num > 0 || (ret == EXIT_FAILURE && error_num != ENOMSG)) {
            (void)msgctl(quid, IPC_RMID, NULL);
        }
    }
    /* Make a failed state explicit if it exists */
    ret = ret != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return ret;
}
