#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdbool.h>
#define SERVER_SOCK_FILENAME "/tmp/unix_server_sock"
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

int run_client(void);

int main(void)
{
    return run_client();
}

int run_client(void)
{
    struct sockaddr_un server;
    int sockfd = -1;
    int status = EXIT_SUCCESS;
    char buf[BUF_SIZ] = {0}, server_filename[FILENAME_SIZ] = {0};
    ssize_t nbytes = 0;
    socklen_t server_len = 0;

    memset(&server, 0, sizeof(struct sockaddr_un));

    if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        handle_error("socket", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    snprintf(server_filename, sizeof(server_filename), "%s", SERVER_SOCK_FILENAME);

    server.sun_family = AF_LOCAL;
    (void)snprintf(server.sun_path, sizeof(server.sun_path), "%s", server_filename);

    server_len = sizeof(server);

    if (connect(sockfd, (struct sockaddr *) &server, server_len) < 0) {
        handle_error("connect", errno);
        status = EXIT_FAILURE;
        goto end;       
    }

    strcpy(buf, "Ping\n");
    nbytes = send(sockfd, buf, strlen("Ping") + 1, 0);
    if (nbytes < 0) {
        handle_error("send", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    printf("Sent \"%s\" to server\n", "Ping");

    if ((nbytes = recv(sockfd, buf, sizeof(buf), 0)) < 0) {
        handle_error("recv", errno);
        status = EXIT_FAILURE;
        goto end;
    } else {
        printf("Received a message of %ld bytes from server '%s'\n", nbytes, server.sun_path);
        if (nbytes > 0) {
            buf[nbytes >= sizeof(buf) ? sizeof(buf) - 1 : nbytes] = '\0';
            printf("Message: %s\n", buf);
        } else
            printf("Empty payload.\n");
    }
end:

    if (sockfd != -1)
        (void)close(sockfd);

    status = status != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return status;
}
