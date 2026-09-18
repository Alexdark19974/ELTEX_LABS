#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#define TRUE 1
#define PORT 1025
#define BUFFER_SIZE 1024
#define FD_OFFSET 500
#define SERV_THREAD_MAX 10
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, " error happened in file %s, line %d.\n", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

typedef struct Threads
{
    pthread_t thread_id;
    struct sockaddr_in *claddr;
    socklen_t claddr_size;
    char *shm_ptr;
    char buffer[BUFFER_SIZE];

} Threads;

void *service(void *const);

void server(void); 

int main(void)
{
    server();

    exit(EXIT_SUCCESS);
}

void server(void)
{
    struct sockaddr_in servaddr, claddr;
    int serv_fd = 0;
    ssize_t status = 0;
    socklen_t servaddr_size = sizeof(struct sockaddr_in);
    socklen_t claddr_size = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE] = {0};

    int log_fd = open("./log.txt", O_CREAT | O_RDWR, 0666);

    if (-1 == log_fd) { handle_error("open"); }

    memset(&servaddr, 0, sizeof(struct sockaddr_in));
    memset(&servaddr, 0, sizeof(struct sockaddr_in));

    struct Threads *client_ptr = NULL;

    if (NULL == (client_ptr = calloc(10, sizeof (struct Threads))))
    {
        handle_error("calloc");
    }

    for (int i = 0; i < 10; i++)
    {
        memset(&client_ptr[i], 0, sizeof(struct Threads));
    }

    shm_unlink("/server.c");

    int shm_fd = shm_open("/server.c", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (-1 == shm_fd) { handle_error("shm_open"); }

    status = ftruncate(shm_fd, BUFFER_SIZE);
    if (-1 == status) { handle_error("shm_open"); }

    char *mem_ptr = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    memset(mem_ptr, 1, 20); // 1 is free, 0 is busy
    memset(mem_ptr + FD_OFFSET, 0, BUFFER_SIZE - FD_OFFSET);

    serv_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == serv_fd) { handle_error("socket"); }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    status = bind(serv_fd, (struct sockaddr *) &servaddr, servaddr_size);
    if (-1 == status) { unsigned short converted_port = ntohs(claddr.sin_port);
        char * converted_ip_addr = inet_ntoa(claddr.sin_addr);
        printf("server: %d port %hu, addr: %s",serv_fd, converted_port, converted_ip_addr); handle_error("bind"); }

    for (int i = 0; i < SERV_THREAD_MAX; i++)
    {
        client_ptr[i].claddr = &claddr;
        client_ptr[i].claddr_size = claddr_size;
        client_ptr[i].shm_ptr = mem_ptr;

        status = pthread_create(&client_ptr[i].thread_id, NULL, &service, &client_ptr[i]);
        if (-1 == status) { handle_error("pthread_create"); }   
    }

    int packages_lost = 0;

    while (TRUE)
    {
        srand( (unsigned int) time(NULL));
        int thread_num = 0;
        int rand_num = rand();

        sleep(1);

        if (0 == rand_num)
        {
            thread_num = 0;
        } 
        else
        {
            thread_num = rand_num % SERV_THREAD_MAX;
        }

        printf("Listening Server%d Waiting for a client...\n", serv_fd);

        ssize_t nbytes = recvfrom(serv_fd, buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &claddr, &claddr_size);

        if (-1 == nbytes) { handle_error("recv_from"); }

        nbytes = sendto(mem_ptr[FD_OFFSET + thread_num], "response", strlen("response"), MSG_CONFIRM, (struct sockaddr *) &claddr, claddr_size);
        memset(&claddr, 0, sizeof(struct sockaddr_in));

        /* 
         * Logging the incorrect work of the threads actually allowed me to see where I've made a mistake.
         * Sendto() tries to send data to the non-existent socket because the calculation of thread_sum is incorrect.
         * % 15 is not correct, since the result reaches beyond the the number of available descriptors (6 - 15)
         * mem_ptr[FD_OFFSET + 12] = 0; (no descriptor);
         * The result is multiple errors in the log file. Descriptors such as -3, 23, 14, 16, while there are only descriptors
         * Ranging from 6 to 15.
         * Solution: % SERV_THREAD_MAX (10).
         *
         * */

        if (-1 == nbytes) 
        { 
            perror("sendto");

            packages_lost++; 

            char tmp_buf[BUFFER_SIZE] = {0};

            snprintf(tmp_buf, BUFFER_SIZE, "Thread : %d\n;", thread_num); 

            status = write(log_fd, tmp_buf, BUFFER_SIZE);

            if (-1 == status)
            {
                handle_error("write");
            }

            sync();

            memset(tmp_buf, 0, BUFFER_SIZE);

            snprintf(tmp_buf, BUFFER_SIZE, "Packages lost: %d\n", packages_lost);

            status = write(log_fd, tmp_buf, BUFFER_SIZE);

            if (-1 == status)
            {
                handle_error("write");
            }

            sync();
        }
    }

    for (int i = 0; i < 10; i++)
    {
        status = pthread_join(client_ptr[i].thread_id, NULL);
        if (-1 == status) { handle_error("pthread_join"); }
    }

    status = close(serv_fd);
    if (-1 == status) { handle_error("close"); }
    serv_fd = 0;

    status = munmap(mem_ptr, BUFFER_SIZE);
    if (-1 == status) { handle_error("close"); }
    mem_ptr = NULL;

    status = shm_unlink("/server.c");
    if (-1 == status) { handle_error("shm_unlink"); }
}

void *service(void *const args)
{
    srand(1026);

    struct Threads *sockaddr_ptr = args;

    struct sockaddr_in servaddr, claddr;

    int serv_fd = 0;
    int counter = 0;
    ssize_t status = 0;
    socklen_t claddr_size = sizeof(struct sockaddr_in);
    unsigned short port = (unsigned short) rand() % 65354;

    memset(&servaddr, 0, sizeof(struct sockaddr_in));
    memset(&claddr, 0, sizeof(struct sockaddr_in));

    serv_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == serv_fd) { handle_error("socket"); }

   for (int l = 0; l < SERV_THREAD_MAX; l++)
    {
        if (0 == sockaddr_ptr->shm_ptr[FD_OFFSET + l])
        {
            sockaddr_ptr->shm_ptr[FD_OFFSET + l] = (char) serv_fd;
            break;
        }
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(port);

    ssize_t nbytes = 0;

    while (TRUE)
    {
        char buffer[BUFFER_SIZE] = {0};

        printf(" Waiting for a client...\n");

        nbytes = recvfrom(serv_fd, buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &claddr, &claddr_size);
        sockaddr_ptr->shm_ptr[serv_fd] = 0;
        counter++;

        printf("buffer[0] - %c\n", buffer[0]);

        if (buffer[0] == '!' && buffer[1] == '\0')
        {
            counter++;
            printf(" %s Server%d is available. Requests served in total: %d\n", sockaddr_ptr->buffer, serv_fd, counter);
            sockaddr_ptr->shm_ptr[serv_fd] = 1;
            continue;
        }

        char *converted_ip_addr = inet_ntoa(claddr.sin_addr);
        unsigned short converted_port = ntohs(claddr.sin_port);
        printf(" Server%d received message %s from a client;\n ip address: %s;\n port: %hu\n", serv_fd, buffer, converted_ip_addr, converted_port);

        nbytes = sendto(serv_fd, "Served you", strlen("Served you"), MSG_CONFIRM, (struct sockaddr  *) &claddr, claddr_size);
        if (-1 == nbytes) 
        { 
            perror("sendto"); 
        }

        printf("Idle.\n");
    }

    status = close(serv_fd);
    if (-1 == status) { handle_error("close"); }
    serv_fd = 0;

    status = munmap(sockaddr_ptr->shm_ptr, BUFFER_SIZE);
    if (-1 == status) { handle_error("munmap"); }

    pthread_exit(NULL);   
}
