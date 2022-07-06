#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#define BUFFER_SIZE 1024
#define SOCK_PATH "/temp"
#define LISTEN_BACKLOG 5
#define TRUE 1
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(void)
{
   struct sockaddr_un serv, client;
   socklen_t client_addr_size = sizeof(struct sockaddr_un);
   int status = 0;
   int serv_fd = 0;
   int client_fd = 0;
   char buffer[BUFFER_SIZE];

   serv_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
   if (-1 == serv_fd) { handle_error("socket"); }

   /*
   		Creating a new socket file decsriptor of AF_LOCAL family 
   		And SOCK_STREAM type
   		And default protocol
   */

   memset(&serv, 0, sizeof(struct sockaddr_un));
   memset(&client, 0, sizeof(struct sockaddr_un));

   /*
   		memset() fills sockaddr_un structs with zeroes
   		Not necessary, but why not? We love 0.
   */

   serv.sun_family = AF_LOCAL;

   strncpy(serv.sun_path, SOCK_PATH, strlen(SOCK_PATH) + 1);
   serv.sun_path[strlen(SOCK_PATH) + 1] = '\0';

   /*
   		Initializing server sockaddr_un struct with family type
   		And strncpying SOCK_PATH where our socket will be located.
   */

   status = bind(serv_fd, (struct sockaddr *) &serv, sizeof(struct sockaddr_un));
   if (status == -1) { handle_error("bind"); }

   /* 
   		Bind() assigns an address to server socket with serv_fd and sockaddr_un struct
   		Using them together.
   */

   status = listen(serv_fd, LISTEN_BACKLOG);
   if (status == -1) { handle_error("listen"); }

   /*
   		listen() turns an active socket into a waiting socket with a waiting queue specified by LISTEN_BACKLOG
   */

   while (TRUE)
   {
   		printf(" Waiting for a client.\n");

   		client_fd = accept(serv_fd, (struct sockaddr *) &(client), &client_addr_size);
		if (-1 == client_fd) { perror("accept"); break; } else { printf(" Client %d connected.\n", client_fd); }

		/* 
			accept() is a blocking call, so server waits until a client tries to connect.
		*/

   		while (TRUE)
   		{
   			status = recv(client_fd, buffer, BUFFER_SIZE, 0);

   			/*
   				recv() will lead to server getting blocked until there's a message from a client
   			*/

   			if (-1 == status) { perror("recv"); break; }
   			if (0 == status) {printf("Client diconnected.\n"); break;}

   			printf(" Server recieved %s\n", buffer);

   			strncat(buffer, "!", 2);

   			printf(" Server sent %s\n", buffer);

   			status = send(client_fd, buffer, BUFFER_SIZE, 0);
   			if (-1 == status) { perror("recv"); }

   			/*
   				Sending a modified string. 
   				If "Hello" is received, then "Hello!" will be send to the client.
   			*/
   		}

   		/* 
   			The server has 2 options:

   			1) continue as before;
   			2) leave and unlink the socket.
   		*/

   		printf ("Do you want to continue waiting for new clients? y/n\n");
   		char micro_buf[2] = {0};

   		fgets(micro_buf, 2, stdin);

   		switch(micro_buf[0])
   		{
   			case 'y':
   			{
   				break;
   			}
   			case 'n':
   			{
   				/* 
   					Freeing the file descriptor 
   				*/

   				status = close(serv_fd);
   				if (status == -1) { handle_error("close"); }
   				break;
   				break;
   			}
   			default:
   			{
   				status = close(serv_fd);
   				if (status == -1) { handle_error("close"); }
   				break;
   			}
   		}
   }

   status = unlink(SOCK_PATH);

   /*
   		unlinking the socket.
   */

   exit(EXIT_SUCCESS);
}