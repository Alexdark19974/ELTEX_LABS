#include "chatroom_header.h"

void cleanup(int qfd_1, char *msg_queue_1, char *msg_queue_2, struct Threads*wait_refresh_ptr, char *buffer)
{
    int state = mq_close(qfd_1);

    if (-1 == state)
    {
         handle_error_en(state, "mq_close");
    }

    state = mq_unlink(msg_queue_1);

    if (-1 == state)
    {
        handle_error_en(state, "mq_unlink");
    }

    state = mq_unlink(msg_queue_2);

    if (-1 == state)
    {
        handle_error_en(state, "mq_unlink");
    }

    free(buffer);
    buffer = NULL;

    free(wait_refresh_ptr);
    wait_refresh_ptr = NULL;

    free(msg_queue_1);
    msg_queue_1 = NULL;

    free(msg_queue_2);
    msg_queue_2 = NULL;
}

