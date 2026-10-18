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
#define LISTEN_BACKLOG 5
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
   struct sockaddr_un server, client;
   int sockfd = -1, c_sockfd = -1;
   int status = EXIT_SUCCESS, i;
   char buf[BUF_SIZ] = {0}, send_buf[BUF_SIZ * 2] = {0}, frame_buf[BUF_SIZ * 2] = {0}, filename[FILENAME_SIZ] = {0};
   char *ch = NULL, *p = NULL;
   ssize_t nbytes = 0, frame_nbytes = 0;
   socklen_t client_len = 0;

   memset(&server, 0, sizeof(struct sockaddr_un));
   memset(&client, 0, sizeof(struct sockaddr_un));

   if ((sockfd = socket(AF_LOCAL, SOCK_STREAM, DEFAULT_PROTO)) < 0) {
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

   if (bind(sockfd, (struct sockaddr *) &server, sizeof(struct sockaddr_un)) < 0) {
      handle_error("bind", errno);
      status = EXIT_FAILURE;
      goto end;
   }


   /* listen() turns an active socket into a waiting socket with a waiting queue specified by LISTEN_BACKLOG */
   if (listen(sockfd, LISTEN_BACKLOG) < 0) {
      handle_error("listen", errno);
      status = EXIT_FAILURE;
      goto end;
   }

   while (true) {
      printf("Waiting for a connection...\n");

      client_len = sizeof(client);
      /* accept() is a blocking call, so server waits until a client tries to connect.*/
      if ((c_sockfd = accept(sockfd, (struct sockaddr *)&client, &client_len)) < 0) {
         handle_error("accept", errno);
         status = EXIT_FAILURE;
         goto end;
      }

      printf("Client %d connected.\n", c_sockfd);

      i = 0;
      while (true) {
         /* server normally blocks on recv() unless there's a message coming in */
         if ((nbytes = recv(c_sockfd, buf, sizeof(buf), 0)) < 0) {
            handle_error("recv", errno);
            status = EXIT_FAILURE;
            goto end;
         } else {
            printf("Received a message of %ld bytes from client %d\n", nbytes, c_sockfd);
            if (nbytes > 0) {
               if ((size_t)(frame_nbytes + nbytes) > sizeof(frame_buf)) {
                  printf("Frame buffer is full becasue the message is too long.\nDrop the connection.\n");
                  (void)close(c_sockfd);
                  c_sockfd = -1;
                  memset(frame_buf, 0, sizeof(frame_buf));
                  frame_nbytes = 0;
                  memset(send_buf, 0, sizeof(send_buf));
                  break;
               }
               memcpy(frame_buf + frame_nbytes, buf, nbytes);
               frame_nbytes += nbytes;
               p = frame_buf;
               ch = (char *)memchr(p, '\n', frame_nbytes);
               if (ch) {
                  size_t len = 0;
                  do {
                     snprintf(send_buf, sizeof(send_buf), "Frame%d: %.*s received!\n", i, (int)(ch - p), p);
                     if ((nbytes = send(c_sockfd, send_buf, strlen(send_buf) + 1, 0)) < 0) {
                        handle_error("send", errno);
                        status = EXIT_FAILURE;
                        goto end;
                     }
                     p = ch + 1; // move onto the next possible delimiter
                     len = (frame_buf + frame_nbytes) - p;
                     i++;
                  } while ((ch = (char *)memchr(p, '\n', len))); // frame_buf + frame_nbytes - p = size of the frame_buf offset
                  if (len) {
                     memmove(frame_buf, p, len);
                     frame_nbytes = len; 
                  } else
                     frame_nbytes = 0;
               }
            } else {
               printf("Client disconnected\n");
               (void)close(c_sockfd);
               c_sockfd = -1;
               memset(frame_buf, 0, sizeof(frame_buf));
               frame_nbytes = 0;
               memset(send_buf, 0, sizeof(send_buf));
               break;
            }
         }
      }
   }

end:
   if (c_sockfd != -1)
      (void)close(c_sockfd);

   if (sockfd != -1)
      (void)close(sockfd);

   if (*filename && unlink(filename) < 0 && errno != ENOENT) {
      handle_error("unlink", errno);
      status = EXIT_FAILURE;
   }

    status = status != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return status;
}