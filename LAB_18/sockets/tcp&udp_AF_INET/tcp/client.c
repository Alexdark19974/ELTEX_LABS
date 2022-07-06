#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BUFFER_SIZE 1024
#define TRUE 1
#define PORT 3889
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void client(void);

int main(void)
{
    client();

    exit(EXIT_SUCCESS);   
}

void client(void)
{
    struct sockaddr_in client;
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    int client_fd = 0;
    int status = 0;
    char buffer[BUFFER_SIZE] = {0};
    char *addr = "127.0.0.1";
    unsigned int net_addr = inet_addr(addr);

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == client_fd) { handle_error("socket"); }

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = net_addr;
    client.sin_port = htons(PORT);

    status = connect(client_fd, (struct sockaddr *) &client, client_addr_size);
    if (-1 == status) { handle_error("connect"); } else { printf(" Connection established.\n"); }

    while (TRUE)
    {
        printf("Enter the message: ");

        fgets(buffer, BUFFER_SIZE, stdin);
        int buf_len = strlen(buffer) -1;
        buffer[buf_len] = '\0';

        if (0 == strncmp(buffer, "close", buf_len) || EOF == buffer[0])
        {
            break;
        }

        status = send(client_fd, buffer, BUFFER_SIZE, MSG_CONFIRM);
        if (-1 == status) { perror("send"); break; }

        status = recv(client_fd, buffer, BUFFER_SIZE, MSG_WAITALL);
        if (-1 == status || 0 == status) { perror("recv"); break; }

        char *converted_ip = inet_ntoa(client.sin_addr);
        unsigned short converted_port = ntohs(client.sin_port);

        printf(" Server recieved %s from client %d with ip address of %s and port of %hu.\n", buffer, client_fd, converted_ip, converted_port);
    }

    status = close(client_fd);
    if (-1 == status) { handle_error("close"); }
}