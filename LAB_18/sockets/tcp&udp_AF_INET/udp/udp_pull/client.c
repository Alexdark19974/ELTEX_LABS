#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <limits.h>

#define TRUE 1
#define BUFFER_SIZE 1024
#define PORT 1025
#define NUMTHREADS 200
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, " error happened in file %s, line %d.\n", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

typedef struct Client_thread
{
    pthread_t thread_id;
    struct sockaddr_in servaddr;
    int client_fd;
    int status;
    ssize_t nbytes;
    socklen_t servaddr_size;
    char buffer[BUFFER_SIZE];
    int counter;
}  Client_thread; 

void client_init(void);
void *call_client(void *const args);

/*
    An inefficient server model. Each client implies a separate server.
    After the client is served, the server is idle so that creates too many servers with too many idle states.
    What we need is:
    1) use the idle servers and figure out a way to implement this;
    2) kill inactive servers after a while;
*/

int main(void)
{
    client_init();

    exit(EXIT_SUCCESS);
}

void client_init(void)
{
    struct Client_thread *client_arr_ptr = NULL;
    int status = 0;
    /*  struct sockaddr_in servaddr;
    int client_fd = 0;
    int status = 0;
    ssize_t nbytes = 0;
    client_params->servaddr_size = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE] = {0};
    int counter = 0;
    */

    if (NULL == (client_arr_ptr = calloc(NUMTHREADS, sizeof(struct Client_thread))))
    {
        handle_error("calloc");
    }

    for (int i = 0; i < NUMTHREADS; i++)
    {
        status = pthread_create(&client_arr_ptr[i].thread_id, NULL, &call_client, &client_arr_ptr[i]);
    }

    for (int i = 0; i < NUMTHREADS; i++)
    {
        status = pthread_join(client_arr_ptr[i].thread_id, NULL);
        if (-1 == status) { handle_error("pthread_join"); }
    }
}

void *call_client(void *const args)
{
    srand(1026);
    struct Client_thread *client_params = args;
    int counter = 0;
    char buffer[BUFFER_SIZE] = {0};
    memset(client_params, 0, sizeof(struct Client_thread));

    client_params->client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == client_params->client_fd) {  handle_error("socket"); }

    client_params->servaddr.sin_family = AF_INET;
    client_params->servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    client_params->servaddr.sin_port = htons(1025);
    client_params->servaddr_size = sizeof(struct sockaddr_in);

    client_params->nbytes = sendto(client_params->client_fd, "ping", strlen("ping"), MSG_CONFIRM, (struct sockaddr *) &client_params->servaddr, client_params->servaddr_size);
    if (-1 == client_params->nbytes) { handle_error("sendto"); } else { printf (" Sent request ping to the server.\n"); }
    memset(&client_params->servaddr, 0, sizeof(struct sockaddr_in));

    client_params->nbytes = recvfrom(client_params->client_fd, buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &client_params->servaddr, &client_params->servaddr_size);
    if (-1 == client_params->nbytes) { handle_error("recvfrom"); } else { printf(" Received a respone \"%s\" from a server.\n", buffer); }
    sleep(1);

    char *converted_ip_addr = inet_ntoa(client_params->servaddr.sin_addr); 
    unsigned short converted_port = ntohs(client_params->servaddr.sin_port);

    printf (" Client%d;\n IP address: %s;\n Port : %hu;\n", client_params->client_fd, converted_ip_addr, converted_port);

    client_params->nbytes = connect(client_params->client_fd, (struct sockaddr *) &client_params->servaddr, client_params->servaddr_size);
    if (-1 == client_params->nbytes) { handle_error("connect"); }

    size_t msg_len = strlen(".Serve me");
    
    while (counter < 5000)
    {
        client_params-> nbytes = send(client_params->client_fd, ".Serve me", msg_len, MSG_CONFIRM);
        if (-1 == client_params->nbytes) { perror("sendto"); }

        client_params->nbytes = recv(client_params->client_fd, buffer, BUFFER_SIZE, MSG_WAITALL);
        if (-1 == client_params->nbytes || 0 == client_params->nbytes) { perror("recv"); continue;} 

        converted_ip_addr = inet_ntoa(client_params->servaddr.sin_addr); 
        converted_port = ntohs(client_params->servaddr.sin_port);
        
        printf(" Client%d received message %s from a server;\n ip address: %s;\n port: %hu\n", client_params->client_fd, buffer, converted_ip_addr, converted_port);
        counter++;
    }

    client_params-> nbytes = send(client_params->client_fd, "!", 2, MSG_CONFIRM);
    if (-1 == client_params->nbytes) { perror("sendto"); }

    sleep(1);

    client_params->nbytes = close(client_params->client_fd);
    if (-1 == client_params->nbytes) { handle_error("close"); }
    client_params->client_fd = 0;

    pthread_exit(NULL);
}
