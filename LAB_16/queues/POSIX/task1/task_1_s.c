#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#define MSG_MAX 8192
#define MAXMSG 10
#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

int main (void)
{
    int state = 0;
    char mtext[MSG_MAX] = {0};
    int prio = 1;

    int fd = open("./message.txt", O_RDONLY);

    if (-1 == fd)
    {
        handle_error_en(fd, "open");
    }

    state = read(fd, mtext, MSG_MAX);

    if (-1 == state)
    {
        handle_error_en(state, "read");
    }

    int qd = mq_open("/my_queue", O_CREAT | O_RDWR | O_NONBLOCK, S_IRUSR | S_IWUSR, NULL);

    if (-1 == qd)
    {
        handle_error_en(qd, "mq_open");
    }

    state = mq_send(qd, mtext, MSG_MAX, prio);

    if (-1 == state)
    {
        handle_error_en(state, "mq_send");
    }

    printf(" Message send\n");

    state = close(fd);
    
    if (-1 == state)
    {
        handle_error_en(state, "open");
    }

    state = mq_close(qd);

    if (-1 == state)
    {
        handle_error_en(state, "mq_close");
    }

    exit(EXIT_SUCCESS);
}
