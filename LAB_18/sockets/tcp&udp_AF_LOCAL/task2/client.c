#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#define CSOCK_PATH "/sock"
#define SOCK_PATH "/temp"
#define BUFFER_SIZE 1024
#define TRUE 1
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

/*	
	Empirically speaking, UDP socket of AF_LOCAL family will not work
	Unless there are 2 different sockets, hence CSOCK_PATH & SOCK_PATH.

	Otherwise client will send a message, the server WILL get it but
	Will not be able to respond and generate an error ENOTCONN

	The way around this is creating 2 independent sockets and then binding
	Both of them to their file descriptors to assgin an address to them. 
*/

void client(void);

int main(void)
{
	client();

	exit(EXIT_SUCCESS);
}

void client(void)
{
	struct sockaddr_un serv, client;
	int client_fd = 0;
	int status = 0;
	char buffer[BUFFER_SIZE] = {0};

	memset(&serv, 0, sizeof(struct sockaddr_un));
	memset(&client, 0, sizeof(struct sockaddr_un));

	/* Creaing a UPD socket */

	client_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if (-1 == client_fd) { handle_error("socket"); }

	/*
		Initializing struct sockaddr_un associated with client. 
	*/

	client.sun_family = AF_LOCAL;
	strncpy(client.sun_path, CSOCK_PATH, strlen(CSOCK_PATH) + 1);
	serv.sun_path[strlen(CSOCK_PATH) + 1] = '\0';

	/* 
		Assgining an address to the socket
	*/

	status = bind(client_fd, (struct sockaddr *) &client, sizeof(struct sockaddr_un));
	if (-1 == status) { handle_error("bind");}

	/*
		Initializing struct sockaddr_un associated with server.
	*/ 

	serv.sun_family = AF_LOCAL;
	strncpy(serv.sun_path, SOCK_PATH, strlen(SOCK_PATH) + 1);
	serv.sun_path[strlen(SOCK_PATH) + 1] = '\0';

	socklen_t serv_addr_size = sizeof(serv);

	/*
		Send some data to the server so that it can know the client.
	*/

	status = sendto(client_fd, "ping", strlen("ping"), MSG_CONFIRM, (struct sockaddr *) &serv, serv_addr_size);
	if (-1 == status) { handle_error("connect"); }

	printf(" Sent ping to the server.\n");

	printf(" Waiting for a response from the server.\n");

	/*
		Wait until "ping" becomes "ping!" which means that the server got the data.
	*/

	status = recvfrom(client_fd, buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &serv, &serv_addr_size);
	if (-1 == status) { handle_error("connect"); }

	/* 
		connect() allows to remeber the server and use send() recv() functons
		Which are traditionally used with TCP socket.
	*/

	status = connect(client_fd, (struct sockaddr *) &serv, sizeof(struct sockaddr_un));
	if (-1 == status) { handle_error("connect"); }

	while (TRUE)
	{
		printf(" Enter the message: ");

		fgets(buffer, BUFFER_SIZE, stdin);

		buffer[strlen(buffer) - 1] = '\0';

		status = send(client_fd, buffer, BUFFER_SIZE, 0);
		if (-1 == status) { handle_error("send"); }

		if (0 == strcmp(buffer, "close"))
		{
			break;
		}

		status = recv(client_fd, buffer, BUFFER_SIZE, 0);
		if (-1 == status) { perror("send"); }

		printf(" Received %s\n", buffer);
	}

	status = close(client_fd);
	if (-1 == status) { handle_error("close"); }

	status = unlink(CSOCK_PATH);
	if (-1 == status) { handle_error("unlink"); }
}
