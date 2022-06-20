#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#define MSG_SIZE 2000
#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)


typedef struct Msg
{
    long mtype;
    char mtext[MSG_SIZE];
} Msg;

int main(void)
{
    int state = 0;
    struct Msg *msg_q_ptr = calloc(1, sizeof(struct Msg));

    if (NULL == msg_q_ptr)
    {
        handle_error("calloc");
    }

    struct msqid_ds buf;

    key_t key = ftok("./task_1.c", 'A');

    if (-1 == key)
    {
        handle_error_en(key, "ftok");
    }

    int qu_id = msgget(key, 0);

    if (-1 == qu_id)
    {
        handle_error_en(key, "msgget");
    }

    state = msgrcv(qu_id, msg_q_ptr, MSG_SIZE, 1, IPC_NOWAIT);

    if (-1 == state)
    {
        handle_error_en(state, "msgrcv");
    }

    state = write(STDOUT_FILENO, msg_q_ptr->mtext, MSG_SIZE);

    if (-1 == state)
    {
        handle_error_en(state, "write");
    }

    state = msgctl(qu_id, IPC_RMID, &buf);

    if (-1 == state)
    {
        handle_error_en(state, "msgctl");
    }

    free(msg_q_ptr);
    msg_q_ptr = NULL;

    exit(EXIT_SUCCESS);
}
