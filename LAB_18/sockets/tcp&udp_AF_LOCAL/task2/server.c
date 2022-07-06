#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#define SOCK_PATH "/temp"
#define BUFFER_SIZE 1024
#define TRUE 1
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
	struct sockaddr_un serv, client;
	int serv_fd = 0;
	int status = 0;
	char buffer[BUFFER_SIZE] = {0};
	socklen_t client_addr_size = sizeof(struct sockaddr_un);

	memset(&serv, 0, sizeof(struct sockaddr_un));
	memset(&client, 0, sizeof(struct sockaddr_un));

	serv_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if (-1 == serv_fd) { handle_error("socket"); }

	serv.sun_family = AF_LOCAL;
	strncpy(serv.sun_path, SOCK_PATH, strlen(SOCK_PATH) + 1);
	serv.sun_path[strlen(SOCK_PATH) + 1] = '\0';

	status = bind(serv_fd, (struct sockaddr *) &serv, sizeof(struct sockaddr_un));
	if (-1 == status) { handle_error("bind"); }

	while (TRUE)
	{
		printf(" Waiting for a client...\n");

		status = recvfrom(serv_fd, buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &client, &client_addr_size);
		if (-1 == status) { handle_error("recvfrom"); }

		if (0 == strcmp(buffer, "close"))
		{
			printf(" Leave? y/n.\n");

			char micro_buf[2] = {0};
			fgets(micro_buf, 2, stdin);

			if (micro_buf[0] == 'y')
			{
				break;
			}
			else 
			{
				continue;
			}
		}

		printf ("Received a message %s from a client of %d bytes, %s.\n", buffer, status, client.sun_path);

		strncat(buffer, "!", 2);

		status = sendto(serv_fd, buffer, BUFFER_SIZE, MSG_CONFIRM, (struct sockaddr *) &client, client_addr_size);
		if (-1 == status) { printf(" %d\n", errno); perror("sendto"); }
	}

	status = close(serv_fd);
	if (-1 == status) { handle_error("close"); }

	status = unlink(SOCK_PATH);
	if (-1 == status) { handle_error("close"); }
}