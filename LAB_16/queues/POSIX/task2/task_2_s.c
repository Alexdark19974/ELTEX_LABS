#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#define MSG_MAX 8192
#define MTEXT_MAX 3
#define FD_MAX 3
#define handle_error_en(en, msg) \
    do { perror(msg); errno = en; fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, " Usage: %s message1.txt message2.txt message3.txt\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int state = 0;
    char mtext[MTEXT_MAX][MSG_MAX] = {0};
    int prio = 0;
    int fd[3] = {0};
    int qfd = mq_open("/queue", O_RDWR | O_NONBLOCK | O_CREAT, S_IRUSR | S_IWUSR, NULL);

    if (-1 == qfd)
    {
        handle_error_en(qfd, "mq_open");
    }

    for (int i = 0; i < FD_MAX; i++)
    {
        prio = i + 1;
        fd[i] = open(argv[i + 1], O_RDONLY);

        if (-1 == fd[i])
        {
            handle_error_en(fd[i], "open");
        }

        state = read (fd[i], mtext[i], MSG_MAX);

        if (-1 == state)
        {
            handle_error_en(state, "read");
        }

        state = close(fd[i]);

        if (-1 == state)
        {
            handle_error_en(fd[i], "close");
        }

        state = mq_send(qfd, mtext[i], MSG_MAX, prio);

        if (-1 == state)
        {
            handle_error_en(state, "mq_send");
        }
        else
        {
            printf(" Message %d sent.\n", prio);
        }

    }

    state = mq_close(qfd);

    if (-1 == state)
    {
        handle_error_en(state, "mq_close");
    }


    exit(EXIT_SUCCESS);
}
