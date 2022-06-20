#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <getopt.h>
#define TRUE 1
#define FALSE 0
#define MSG_SIZE 100
#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

typedef struct Msg
{
    long mtype;
    char mtext[MSG_SIZE];
} Msg;

int main(int argc, char *argv[])
{
    int state = 0;
    key_t key = 0;
    int qu_id = 0;
    int opt = 0;
    int send = 0;
    int receive = 0;
    struct Msg *msg_ptr = NULL;

    while ((opt = getopt(argc, argv, "sr")) != -1) // NEVER type "sr: with ':' unless you want some option added after it!!!"
    {
        switch (opt)
        {
            case 's':
            {
                send = TRUE;
                break;
            }
            case 'r':
            {
                receive = TRUE;
                break;
            }
            default:
            {
                fprintf(stderr, " Usage: %s -s OR -r\n", argv[0]);
                exit(EXIT_FAILURE);
            }
        }
    }

    msg_ptr = calloc(1, sizeof(struct Msg));

    if (NULL == msg_ptr)
    {
        handle_error("calloc");
    }

    key = ftok("./task_3_rs.c", 'C');

    if (TRUE == send && !receive)
    {
        msg_ptr->mtype = 1;

        state = read(STDIN_FILENO, msg_ptr->mtext, MSG_SIZE);

        if (-1 == state)
        {
            handle_error_en(state, "read");
        }

        qu_id = msgget(key, 0666 | IPC_CREAT);

        if (-1 == qu_id)
        {
            handle_error_en(qu_id, "msgget");
        }

        state = msgsnd(qu_id, msg_ptr, MSG_SIZE, IPC_NOWAIT);

        if (-1 == state)
        {
            handle_error_en(state , "msgsnd");
        }
    }

    if (TRUE == receive && !send)
    {
        struct msqid_ds buf;

        qu_id = msgget(key, 0);

        if (-1 == qu_id)
        {
            handle_error_en(qu_id, "msgget");
        }

        state = msgrcv(qu_id, msg_ptr, MSG_SIZE, 1, IPC_NOWAIT);

        if (-1 == state)
        {
            handle_error_en(state, "msgrcv");
        }

        state = write(STDOUT_FILENO, msg_ptr->mtext, MSG_SIZE);

        if (-1 == state)
        {
            handle_error_en(state, "write");
        }

        state = msgctl(qu_id, IPC_RMID, &buf);

        if (-1 == state)
        {
            handle_error_en(state, "msgctl");
        }
    }

    free(msg_ptr);
    msg_ptr = NULL;

    exit(EXIT_SUCCESS);
}
