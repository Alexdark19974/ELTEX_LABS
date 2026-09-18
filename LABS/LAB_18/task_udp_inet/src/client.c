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
#define TRUE 1
#define BUFFER_SIZE 1024
#define PORT 7777
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, " error happened in file %s, line %d.\n", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

void client(void);

int main(void)
{
    client();

    exit(EXIT_SUCCESS);
}

void client(void)
{
    struct sockaddr_in servaddr;
    int client_fd = 0;
    int status = 0;
    ssize_t nbytes = 0;
    socklen_t servaddr_size = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE] = {0};

    memset(&servaddr, 0, sizeof(struct sockaddr_in));

    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == client_fd) {  handle_error("socket"); }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    nbytes = sendto(client_fd, "ping", strlen("ping"), MSG_CONFIRM, (struct sockaddr *) &servaddr, servaddr_size);
    if (-1 == nbytes) { handle_error("sendto"); } else { printf (" Sent request to the server.\n"); }

    nbytes = recvfrom(client_fd, buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &servaddr, &servaddr_size);
    if (-1 == nbytes) { handle_error("recvfrom"); } else { printf(" Received a respone from a client.\n"); }

    char *converted_ip_addr = inet_ntoa(servaddr.sin_addr); 
    unsigned short converted_port = ntohs(servaddr.sin_port);

    printf (" ip address: %s;\n port : %hu;\n", converted_ip_addr, converted_port);

    status = connect(client_fd, (struct sockaddr *) &servaddr, servaddr_size);
    if (-1 == status) { handle_error("connect"); }

    while (TRUE)
    {
        printf(" Enter the message: ");

        fgets(buffer, BUFFER_SIZE, stdin);

        size_t buf_len = strlen(buffer) - 1;
        buffer[buf_len] = '\0';

        nbytes = send(client_fd, buffer, BUFFER_SIZE, MSG_CONFIRM);
        if (-1 == nbytes) { perror("sendto"); }

        if (0 == strncmp(buffer, "close", buf_len) || EOF == buffer[0])
        {
            break;
        }

        nbytes = recv(client_fd, buffer, BUFFER_SIZE, MSG_WAITALL);
        if (-1 == nbytes || 0 == nbytes) { perror("recv"); } 

        converted_ip_addr = inet_ntoa(servaddr.sin_addr); 
        converted_port = ntohs(servaddr.sin_port);
        
        printf(" Client received message %s from a client;\n ip address: %s;\n port: %hu\n", buffer, converted_ip_addr, converted_port);
    }

    status = close(client_fd);
    if (-1 == status) { handle_error("close"); }
}