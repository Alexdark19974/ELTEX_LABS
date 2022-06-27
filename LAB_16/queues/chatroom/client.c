 #include "chatroom_header.h"

int main(void)
{
    char msg[MSG_MAX] = {0};
    int state = 0;
    int my_pid = getpid();
    char my_pid_str[MSG_MAX] = {0};
    int length = snprintf(NULL, 0, "%d", my_pid);

     if (-1 == state)
     {
            perror("snprintf");
     }

    state = snprintf(my_pid_str, length + 1, "%d", my_pid);

    if (-1 == state)
    {
        perror("snprintf");
    }

    printf(" Connecting to the server...\n");
    sleep(5);

    int qfd = mq_open("/session_queue", O_RDWR /*| O_NONBLOCK*/);
    int qfd2 = mq_open("/special_queue", O_RDWR /*| O_NONBLOCK*/);

    if (-1 == qfd || -1 == qfd2)
    {
        printf(" No response from the server. Please, try again later.\n");
        sleep(2);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf(" Successfully connected.\n");
        state = mq_send(qfd2, my_pid_str, MSG_MAX, 2);
    }

    while (TRUE)
    {
        printf("Enter the message: ");
        fflush(stdout);
        sync();

        fgets(msg, MSG_MAX, stdin);


        if (msg[0] == 'q' && msg[1] == '\n')
        {
            state = mq_send(qfd2, "disconnect", MSG_MAX, 1);
            break;
        }


        unsigned int prio = 0;
        char buf[MSG_MAX] = {0};

        char mdata[MSG_MAX] = {0};

        state = mq_send(qfd2, msg, MSG_MAX, 1);

            if (msg[0] == 'q' && msg[1] == '\n')
            {
                break;
            }

        if (-1 == state)
        {
            handle_error_en(state, "mq_send");
        }
        else
        {
            printf(" Sent the message to the server.\n");
            sleep(2);
        }

        printf("waiting for the response from the server\n");
        sleep(5);

        state = mq_receive(qfd, mdata, MSG_MAX, &prio);

        if (0 == strcmp (mdata, "password required"))
        {
            printf("enter the password: ");

            fgets(buf, 8, stdin);

            mq_send(qfd2, buf, MSG_MAX, 1);

            for (int i = 0; mdata[i] != '\0'; i++)
            {
                mdata[i] = '\0';
            }

            printf("waiting for the response from the server\n");

            state = mq_receive(qfd, mdata, MSG_MAX, &prio);
        }

        if (0 == strcmp(mdata, "Granted"))
        {
            state = mq_receive(qfd, mdata, MSG_MAX, &prio);

            printf(" Obtained %s from the server\n", mdata);

            state = mq_send(qfd2, "Success", MSG_MAX, 1);

            printf("Sent \"Success\" to the server\n" );

            sleep(5);
        }
        else if (0 != "rejected")
        {
            printf("Got %s from the server\n", mdata);
            sleep(5);
        }
        else
        {
            printf ("Got rejected from the server\n");
        }
    }

    state = mq_close(qfd2);

    if (-1 == state)
    {
            handle_error_en(state, "close");
    }

    state = mq_close(qfd);

    if (-1 == state)
    {
        handle_error_en(state, "close");
    }

    printf(" Disconnecting...\n");

    sleep(2);

    exit(EXIT_SUCCESS);
}
