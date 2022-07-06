#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define TRUE 1
#define PORT 7777
#define BUFFER_SIZE 1024
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, " error happened in file %s, line %d.\n", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

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

    memset(&servaddr, 0, sizeof(struct sockaddr_in));
    memset(&servaddr, 0, sizeof(struct sockaddr_in));

    serv_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == serv_fd) { handle_error("socket"); }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    status = bind(serv_fd, (struct sockaddr *) &servaddr, servaddr_size);
    if (-1 == status) { handle_error("bind"); }

    while (TRUE)
    {
        printf(" Waiting for a client...\n");

        ssize_t nbytes = recvfrom(serv_fd, buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &claddr, &claddr_size);
        if (-1 == nbytes || 0 == nbytes) { perror("recvfrom"); continue; }
        else
        {
            char *converted_ip_addr = inet_ntoa(claddr.sin_addr);
            unsigned short converted_port = ntohs(claddr.sin_port);
            printf(" Server received message %s from a client;\n ip address: %s;\n port: %hu\n", buffer, converted_ip_addr, converted_port);
        }

        if (0 == strncmp("close", buffer, strlen(buffer)) || EOF == buffer[0])
        {
            break;
        }

        strncat(buffer, "!", 2);

        nbytes = sendto(serv_fd, buffer, BUFFER_SIZE, MSG_CONFIRM, (struct sockaddr  *) &claddr, claddr_size);
        if (-1 == nbytes) { perror("sendto"); }
    }

    status = close(serv_fd);
    if (-1 == status) { handle_error("close"); }
}