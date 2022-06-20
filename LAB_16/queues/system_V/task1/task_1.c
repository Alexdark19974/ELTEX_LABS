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
    int fd = open("./message.txt", O_RDONLY);

    if (NULL == msg_q_ptr)
    {
        handle_error("calloc");
    }

    msg_q_ptr->mtype = 1;

    state = read(fd, msg_q_ptr->mtext, MSG_SIZE);

    if (-1 == state)
    {
        handle_error_en(state, "read");
    }
    
    key_t q_key = ftok("./task_1.c", 'A');

    if (-1 == q_key)
    {
        handle_error_en(q_key, "ftok");
    }

    int qu_id = msgget(q_key, 0666 | IPC_CREAT);

    if (-1 == qu_id)
    {
        handle_error_en(state, "msgget");
    }

    state = msgsnd(qu_id, msg_q_ptr, MSG_SIZE, 0);

    if (-1 == state)
    {
        handle_error_en(state, "msgsnd");
    }
    else
    {
        state = write(STDOUT_FILENO, " Message sent\n", 15);

        if (-1 == state)
        {
            handle_error_en(state, "msgsnd");
        }
    }

    free(msg_q_ptr);
    msg_q_ptr = NULL;

    state = close(fd);

    if (-1 == state)
    {
        handle_error_en(state, "close");
    }

    exit(EXIT_SUCCESS);
}
