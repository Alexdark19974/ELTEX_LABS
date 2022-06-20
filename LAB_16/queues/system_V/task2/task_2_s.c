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
    char mtext[MSG_SIZE];
} Msg;

int main (void)
{
    int state = 0;
    key_t key = 0;
    int qu_id = 0;
    int fd[FD_MAX] = {0}; 

    struct Msg *msg_ptr[MSG_MAX] = {NULL};

    for (int i = 0; i < MSG_MAX; i++)
    {
        msg_ptr[i] = calloc(1, sizeof(struct Msg));
    } 

    fd[0] = open("./message_1.txt", 0666 | O_RDONLY);

    if (-1 == fd[0])
    {
        handle_error_en(fd[0], "open");
    }

    fd[1] = open("./message_2.txt", 0666 | O_RDONLY);
    
    if (-1 == fd[1])
    {
        handle_error_en(fd[1], "open");
    }

    for (int i = 0; i < MSG_MAX; i++)
    {
        msg_ptr[i]->mtype = i + 1;

        state = read(fd[i], msg_ptr[i]->mtext, MSG_SIZE);

        if (-1 == state)
        {
            handle_error_en(state, "read");
        }

        state = close(fd[i]);

        if (-1 == state)
        {
            handle_error_en(state, "close");
        }
    }

    key = ftok("./task_2_s.c", 'B');

    if (-1 == key)
    {
        handle_error_en(key, "ftok");
    }

    qu_id = msgget(key, 0666 | IPC_CREAT);

    for (int i = 0; i < MSG_MAX; i++)
    {
        state = msgsnd(qu_id, msg_ptr[i], MSG_SIZE, IPC_NOWAIT);

        if (-1 == state)
        {
            handle_error_en(state, "msgsnd");
        }

        state = write(STDOUT_FILENO, " message send\n", 15);

        if (-1 == state)
        {
            handle_error_en(state, "write");
        }

        free(msg_ptr[i]);
        msg_ptr[i] = NULL;
    }

    exit(EXIT_SUCCESS);
}
