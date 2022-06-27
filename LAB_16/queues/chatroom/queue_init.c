#include "chatroom_header.h"

int message_queue_init(char *msg_q_name)
{
    int qfd = mq_open(msg_q_name, O_CREAT | O_RDWR /*| O_NONBLOCK*/, S_IRUSR | S_IWUSR, NULL);

    if (-1 == qfd)
    {
        handle_error_en(qfd, "mq_open");
    }

    return qfd;
}
