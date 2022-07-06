#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#define SOCK_PATH "/temp"
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define BUFFER_SIZE 1024
#define TRUE 1

void client(void);

int main(void)
{
	client();

	exit(EXIT_SUCCESS);
}

void client(void)
{
	struct sockaddr_un client;
	int client_fd;
	int status = 0;
	char buffer[BUFFER_SIZE] = {0};

	client_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (-1 == client_fd) { handle_error("socket"); }

	memset(&client, 0, sizeof(struct sockaddr_un));

	client.sun_family = AF_LOCAL;

	strncpy(client.sun_path, SOCK_PATH, strlen(SOCK_PATH) + 1);
	client.sun_path[strlen(SOCK_PATH) + 1] = '\0';

	status = connect(client_fd, (struct sockaddr *) &client, sizeof(struct sockaddr_un));
	if (-1 == status) { handle_error("connect"); }

	/* 
		Connecting to the socket of the server
		Establishing peer-to-server connection.
	*/

	while (TRUE)
	{
		printf("Enter the message: ");

		fgets(buffer, BUFFER_SIZE, stdin);

		buffer[strlen(buffer) - 1] = '\0';

		if (0 == strcmp(buffer, "close"))
		{
			/* 
				Upon typing "close"
				The client will be terminated 
				And the server will be notified.
			*/

			break;
		}

		status = send(client_fd, buffer, BUFFER_SIZE, 0);
		if (-1 == status) { handle_error("send"); }

		/* 
			OG string 
		*/

		status = recv(client_fd, buffer, BUFFER_SIZE, 0);
		if (-1 == status) { handle_error("send"); }

		/*
			Modified String from the server
		*/

		printf(" Client received %s\n", buffer);
	}

	close(client_fd);
}