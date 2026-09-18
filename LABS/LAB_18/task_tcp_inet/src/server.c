#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#define BUFFER_SIZE 1024
#define BACK_LOG 5
#define TRUE 1
#define PORT 3889
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void server(void);

int main(void)
{
    server();
    exit(EXIT_SUCCESS);
}

void server(void)
{
    struct sockaddr_in serv, client;
    int serv_fd = 0;
    int client_fd = 0;
    int status = 0;
    char buffer[BUFFER_SIZE] = {0};
    char *addr = "127.0.0.1";
    unsigned int net_addr = inet_addr(addr);
    socklen_t client_addr_size = sizeof(struct sockaddr_in);
    socklen_t serv_addr_size = sizeof(struct sockaddr_in);

    memset(&serv, 0, sizeof(struct sockaddr_in));
    memset(&client, 0, sizeof(struct sockaddr_in));

    serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == serv_fd) { handle_error("socket"); }

    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = net_addr;
    serv.sin_port = htons(PORT);

    status = bind(serv_fd, (struct sockaddr *) &serv, serv_addr_size);
    if (-1 == status) { handle_error("bind"); }

    status = listen(serv_fd, BACK_LOG);
    if (-1 == status) { handle_error("listen"); }

    while (TRUE)
    {
        printf(" Waiting for a client...\n");

        client_fd = accept(serv_fd, (struct sockaddr *) &client, &client_addr_size);
        if (-1 == client_fd) { perror("accept"); break; } else { printf(" Client connected.\n"); }

        while (TRUE)
        {
            status = recv(client_fd, buffer, BUFFER_SIZE, MSG_WAITALL);
            if (-1 == status) { perror("recv"); break; }

            if (0 == status)
            {
                printf(" Client disconnected.\n");

                char micro_buf[2] = {0};

                printf(" Try to wait for another client? y/n.\n");
                
                fgets(micro_buf, 2, stdin);

                if ('y' == micro_buf[0])
                {
                    break;
                }
                else
                {
                    status = close(serv_fd);
                    if (-1 == status) { handle_error("close");}
                    break;
                }
            }
            else
            {
                char *converted_ip = inet_ntoa(client.sin_addr);
                unsigned short converted_port = ntohs(client.sin_port);
                printf(" Server recieved %s from client %d with ip address of %s and port of %hu.\n", buffer, client_fd, converted_ip, converted_port);       
            }

            strncat(buffer, "!", 2);

            status = send(client_fd, buffer, BUFFER_SIZE, MSG_CONFIRM);
            if (-1 == status) { perror("send"); }
        }
    }
}