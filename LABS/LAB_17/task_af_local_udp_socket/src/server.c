#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdbool.h>
#define SOCK_PATH "/tmp/unix_server_sock"
#define BUF_SIZ 256
#define FILENAME_SIZ 108
#define DEFAULT_PROTO 0
#define handle_error(msg, errnum) do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = NULL; \
                                    if (!(err_str = strerror_r((errnum), buf, sizeof(buf)))) \
                                        fprintf(stderr, "error: failed to print error message!\n"); \
                                    else \
                                        fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)
#define handle_error_msg(fmt, ...) do { \
                            fprintf(stderr, fmt, __VA_ARGS__); \
                        } while(0)

int run_server(void);

int main(void)
{
	return run_server();
}

int run_server(void)
{
	struct sockaddr_un server, client;
	int sockfd = -1;
	int status = EXIT_SUCCESS;
	char buf[BUF_SIZ] = {0}, filename[FILENAME_SIZ] = {0};
	ssize_t nbytes = 0;
	socklen_t client_len = 0;

	memset(&server, 0, sizeof(struct sockaddr_un));
	memset(&client, 0, sizeof(struct sockaddr_un));

	/* domain: defines a socket or address family which is a specific name space.
		 AF_LOCAL or AF_UNIX: unix domain sockets are used for local interprocess communication. See unix(7) for more details */
	/* type: defines communication semantics or a socket type.
		unix domain sockets consists of 3 socket types: SOCK_STREAM, SOCK_DGRAM, and SOCK_SEQPACKET */
	/* protocol: defines a protocol within a given adress family
		Usually, there's only one protocol wthin a given family, so protocol is set to 0 */
	/* A Unix-domain socket is just an fd until you give it a name other processes can find. */
	if ((sockfd = socket(AF_LOCAL, SOCK_DGRAM, DEFAULT_PROTO)) < 0) {
		handle_error("socket", errno);
		status = EXIT_FAILURE;
		goto end;
	}

	snprintf(filename, sizeof(filename), "%s", SOCK_PATH);
	/* unlink the filename so we can avoid "Address already in use from bind()"*/
	if (unlink(filename) < 0 && errno != ENOENT) {
		handle_error("unlink", errno);
		status = EXIT_FAILURE;
		*filename = '\0';
		goto end;
	}


	server.sun_family = AF_LOCAL;
	(void)snprintf(server.sun_path, sizeof(server.sun_path), "%s", filename);

	/* bind unix domain socket to a filename */
	/* Address format
       A UNIX domain socket address is represented in the following structure: */
           // struct sockaddr_un {
               // sa_family_t sun_family;               /* AF_UNIX */
               // char        sun_path[108];            /* Pathname */
           // };
	/* The rules used in name binding vary between address families */
	/* socket(AF_LOCAL, SOCK_DGRAM, 0) creates an unnamed socket. The kernel knows that fd. Nobody else does. There is no IP:port.
		bind() attaches a pathname (/tmp/unix_server_sock) to that fd. That creates a special file in the filesystem.
		A client then sendto()/connect() to that path, and the kernel delivers the datagram to your bound socket. */
	/* so binding to a pathname is like binding to ip:port for a network socket. A client can identify a server by its name, connect to it and send datagrams. */	
	if (bind(sockfd, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
		handle_error("bind", errno);
		status = EXIT_FAILURE;
		goto end;
	}


	/* The socket is a kernel object. You hold it as an fd from socket(). That is what recvfrom / sendto use.
		The filename (/tmp/unix_server_sock) is a directory entry. bind() creates that special file (srw-…) and attaches your fd to it.
		Other processes use the path only to find the socket. */
	while (true) {
		printf("Awaiting messages...\n");

		client_len = sizeof(client); // set client_len every time before recvfrom() and after sendto() to tell the kernel about our buffer size
		/* Before the call, it should be initialized to the size of the buffer associated with client.
			Upon return, addrlen is updated to contain the actual size of the client. */
		if ((nbytes = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &client_len)) < 0) {
			handle_error("recvfrom", errno);
			status = EXIT_FAILURE;
			goto end;
		} else {
			printf("Received a message of %ld bytes from client '%s'\n", nbytes, client.sun_path);
			if (nbytes > 0) {
				buf[nbytes >= sizeof(buf) ? sizeof(buf) - 1 : nbytes] = '\0';
				printf("Message: %s\n", buf);
			} else
				printf("Empty payload.\n");

			strcpy(buf, "Message received!");
			nbytes = sendto(sockfd, buf, strlen("Message received!") + 1, 0, (struct sockaddr *)&client, client_len);
			if (nbytes < 0) {
				handle_error("sendto", errno);
				status = EXIT_FAILURE;
				goto end;
			}

			printf("Sent \"%s\" to client\n", "Message received!");
		}
	}

end:
	if (sockfd != -1)
		(void)close(sockfd);

	if (*filename && unlink(filename) < 0 && errno != ENOENT) {
		handle_error("unlink", errno);
		status = EXIT_FAILURE;
	}

    status = status != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return status;
}