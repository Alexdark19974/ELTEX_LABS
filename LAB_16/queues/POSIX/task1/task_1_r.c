#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#define MSG_MAX 8192 // standard MSG_MAX. can be changed by using mq_setattr()
#define handle_error_en(en, msg) \
    do { perror(msg); errno = en; fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

int main(void)
{
    int state = 0;
    char mtext[MSG_MAX] = {0};
    unsigned int prio = 0;
    int qd = mq_open("/my_queue", O_RDWR | O_NONBLOCK);

    if (-1 == qd)
    {
        handle_error_en(qd, "mq_open");
    }

    state = mq_receive(qd, mtext, MSG_MAX, &prio);

    if (-1 == state)
    {
        perror("mq_receive");
        handle_error_en(state, "mq_receive");
    }

    printf(" priority is %d\n", prio);

    sleep(2);

    state = write(STDOUT_FILENO, mtext, MSG_MAX);

    if (-1 == state)
    {
        handle_error_en(state, "write");
    }

    state = mq_close(qd);

    if (-1 == state)
    {
        handle_error_en(state, "mq_close");
    }

    state = mq_unlink("/my_queue");

    if (-1 == state)
    {
        handle_error_en(state, "mq_unlink");
    }

    exit(EXIT_SUCCESS);
}
