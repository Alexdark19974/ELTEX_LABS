#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#define MSG_MAX 8192
#define MTEXT_MAX 3
#define handle_error_en(en, msg) \
    do { perror(msg); errno = en; fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

int main(void)
{
    int state = 0;
    char mtext[MTEXT_MAX][MSG_MAX] = {0};
    int qfd = mq_open("/queue", O_RDONLY);

    if (-1 == qfd)
    {
        handle_error_en(qfd, "mq_open");
    }

    unsigned int prio[MTEXT_MAX] = {0};

    for (int i = 0; i < MTEXT_MAX; i++)
    {
        state = mq_receive(qfd, mtext[i], MSG_MAX, &prio[i]);

        if (-1 == state)
        {
            handle_error_en(state, "mq_receive");
        }
        
        printf(" --- THIS IS A MESSAGE with priority %d\n", prio[i]);
        sync();
        state = write(STDOUT_FILENO, mtext[i], MSG_MAX);

        if (-1 == state)
        {
            handle_error_en(state, "write");
        }

    }

    state = mq_close(qfd);

    if (-1 == state)
    {
        handle_error_en(state, "mq_close");
    }


    state = mq_unlink("/queue");

    if (-1 == state)
    {
        handle_error_en(state, "mq_unlink");
    }

    exit(EXIT_SUCCESS);
}
