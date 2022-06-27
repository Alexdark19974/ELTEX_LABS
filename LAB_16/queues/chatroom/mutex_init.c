#include "chatroom_header.h"

extern pthread_mutex_t mutex;

void mutex_init(void)
{
    int state = pthread_mutex_init(&mutex, NULL);

    if (-1 == state)
    {
        handle_error_en(state, "pthread_mutex_init");
    }
}

