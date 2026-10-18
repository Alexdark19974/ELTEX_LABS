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
    ssize_t nbytes = -1;
    Msg msg = {0, {0}};
    int quid = -1;
    int errnum = 0;
    int ret = EXIT_SUCCESS;
    key_t key;

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

    // read message of type MSG_TYPE
    if ((nbytes = msgrcv(quid, (void *)&msg, sizeof(msg.mtext) - 1, MSG_TYPE, IPC_NOWAIT)) < 0) {
        /* store the value of errno immediately because other library calls may modify it */
        errnum = errno;
        if (errnum != ENOMSG) {
            handle_error("msgrcv", errnum);
            handle_error_msg("Failed to read message from %s\n", MESSAGE);
            // (void)msgctl(quid, IPC_RMID, NULL); do not delete queue on failure; let other receivers do the job
            ret = EXIT_FAILURE;
        }
    } else {
        printf("read %ld bytes from queue%d\n", nbytes, quid);
        printf("Message:\n");
        printf("%s\n", msg.mtext);
    }
    // msg.mtext[nbytes == MSG_SIZE ? MSG_SIZE : nbytes] = '\0'; // not needed since we have an extra byte that is zeroed out. 
fail:
    if (quid != -1) {
        /* we read something or payload is zero OR if it's an error other than ENOMSG, we forecfully remove the message queue */
        if (nbytes >= 0 || (ret == EXIT_FAILURE && errnum != ENOMSG))
            (void)msgctl(quid, IPC_RMID, NULL);
    }
    /* Make a failed state explicit if it exists */
    ret = ret != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return ret;
}
