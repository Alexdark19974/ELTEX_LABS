#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <malloc.h>
#define MSG_SIZE 2000
#define MSG_MAX 2
#define FD_MAX 2
#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

typedef struct Msg
{
    long mtype;
    long mtext[MSG_SIZE];
} Msg;

int main(void)
{
    int state = 0;
    key_t key = 0;
    int qu_id = 0;
    struct msqid_ds buf;
    struct Msg *msg_ptr[MSG_MAX] = {NULL};

    for (int i = 0; i < MSG_MAX; i++)
    {
        msg_ptr[i] = calloc(1, sizeof(struct Msg));

        if (NULL == msg_ptr[i])
        {
            handle_error("calloc");
        }
    }

    key = ftok("./task_2_s.c", 'B');

    if (-1 == key)
    {
        handle_error_en(key, "ftok");
    }

    qu_id = msgget(key, 0);

    if (-1 == qu_id)
    {
        handle_error_en(qu_id, "msgget");
    }

    for (int i = 0; i < MSG_MAX; i++)
    {
        state = msgrcv(qu_id, msg_ptr[i], MSG_SIZE, 2 - i, IPC_NOWAIT);

        if (-1 == state)
        {
            handle_error_en(state, "msgrcv");
        }
    }

    for (int i = 0; i < MSG_MAX; i++)
    {
        state = write(STDOUT_FILENO, msg_ptr[i], MSG_SIZE);

        if (-1 == state)
        {
            handle_error_en(state, "write");
        }
    }

    state = msgctl(qu_id, IPC_RMID, &buf);

    if (-1 == state)
    {
        handle_error_en(state, "msgctl");
    }

    for (int i = 0; i < MSG_MAX; i++)
    {
        free(msg_ptr[i]);
        msg_ptr[i] = NULL;
    }

    exit(EXIT_SUCCESS);
}
