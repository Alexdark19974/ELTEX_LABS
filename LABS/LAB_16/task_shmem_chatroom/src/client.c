 #include "chatroom_header.h"

void client(void);

int main(void)
{
    client();
    exit(EXIT_SUCCESS);
}

void client(void)
{
    char msg[SEGSIZE] = {0};
    int state = 0;
    int my_pid = getpid();
    char my_pid_str[SEGSIZE] = {0};

    int length = snprintf(NULL, 0, "%d", my_pid);
    error_check(length, "snprintf");

    state = snprintf(my_pid_str, length + 1, "%d", my_pid);
    error_check(length, "snprintf");

    printf(" Connecting to the server...\n");

    int shmfd_1 = shm_open("/chatroom_server.c", O_RDWR, 0666);
 
    if (-1 == shmfd_1)
    {
        printf(" No response from the server. Please, try again later.\n");
        exit(EXIT_FAILURE);
    }

    char *shm_ptr_1 = mmap(NULL, SEGSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd_1, 0);
    if (MAP_FAILED == shm_ptr_1) { error_check_ptr("mmap"); }

    strncpy(shm_ptr_1, "PID ", strlen("PID ") + 1);
    strncat(shm_ptr_1, my_pid_str, strlen(my_pid_str));

    sem_t *semaphore_2 = sem_open("client.c", 0);
    if (SEM_FAILED == semaphore_2) { error_check_ptr("sem_open"); }

    state = sem_wait(semaphore_2);
    error_check(state, "sem_wait");

    state = sem_wait(semaphore_2);
    error_check(state, "sem_wait");

    printf(" Successfully connected.\n");

    while (TRUE)
    {
        printf("Enter the message: ");

        fgets(msg, SEGSIZE, stdin);
        int length_2 = strlen(msg);
        msg[length_2 - 1] = '\0';

        if (msg[0] == 'q' && msg[1] == '\0')
        {
            strncpy(shm_ptr_1, "disconnect", strlen("disconnect") + 1);
            shm_ptr_1[strlen("disconnect") + 1] = '\0';
            printf("%s\n", shm_ptr_1);
            break;
        }

        strncpy(shm_ptr_1, msg, strlen(msg) + 1);
        shm_ptr_1[strlen(msg) + 1] = '\0';

        if (msg[0] == 'q' && msg[1] == '\n')
        {
            printf("%s\n", shm_ptr_1);
            break;
        }
        else
        {
            printf(" Sent the message to the server.\n");
            sleep(2);
        }

        printf("waiting for the response from the server\n");
        state = sem_wait(semaphore_2);
        error_check(state, "sem_wait");
 
        if (0 == strcmp (shm_ptr_1 + 500, "password required"))
        {
            printf("enter the password: ");

            fgets(shm_ptr_1, 8, stdin);

            int len = strlen(shm_ptr_1);
            shm_ptr_1[len - 1] = '\0';

            state = sem_wait(semaphore_2);
            error_check(state, "sem_wait");

            printf("waiting for the response from the server\n");
       
        }

        if (0 == strcmp(shm_ptr_1 + 500, "Granted"))
        {
            printf("waiting for the response from the server\n");

            state = sem_wait(semaphore_2);
            error_check(state, "sem_wait");
 
            printf(" Obtained %s from the server\n", shm_ptr_1 + 500);

            strncpy(shm_ptr_1, "Success", strlen("Success") + 1);
            shm_ptr_1[strlen("Success") + 1] = '\0';

            sleep(5);
        }
        else if (0 == strcmp(shm_ptr_1 + 500, "Rejected"))
        {
            printf ("Got rejected from the server\n");
        }
        else if (0 == strcmp (shm_ptr_1 + 500, "server died"))
        {
            break;
        }
        else
        { 
            printf("Got %s from the server\n", shm_ptr_1 + 500);
            clean_string(shm_ptr_1 + 500);
        }
    }

    printf(" Disconnecting...\n");

    sleep(2);

    state = munmap(shm_ptr_1, SEGSIZE);
    error_check(state, "munmap");

    state = sem_close(semaphore_2);
    error_check(state, "sem_close");
}

void clean_string(char *buffer_to_be_cleaned)
{
    for (int j = 0; buffer_to_be_cleaned[j] != '\0'; j++)
    {
        buffer_to_be_cleaned[j] = '\0';
    }
}