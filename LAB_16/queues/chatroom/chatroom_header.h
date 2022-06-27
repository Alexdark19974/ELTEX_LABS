#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <curses.h>
#include <pthread.h>
#define MSG_MAX 8192
#define THREAD_MAX 2
#define MAX_USR 100
#define TRUE 1
#define FALSE 0
#define handle_error_en(en, msg) \
    do { perror(msg); errno = en; fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
    do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

typedef struct Threads
{
    pthread_t thread_id;
    int qu_fd;
    int qu_fd_2;
    char *buffer_ptr;
    int *user_pid_ptr;
    WINDOW *chit_chat_wnd_ptr;
    WINDOW *user_list_wnd_ptr;
    WINDOW *tab_wnd_ptr;
} Threads;

void chatroom_server_init(void);
void sig_winch(int);
WINDOW *init_tab_wnd(void);
WINDOW *init_chit_chat_wnd(void);
WINDOW *init_user_list_wnd(void);
int message_queue_init(char *);
void *refresh_chat_room(void *const args);
void delete_windows(WINDOW *, WINDOW *, WINDOW *);
void *server_control_room(void *const args);
void cleanup(int, char *, char *, struct Threads *, char *);
void mutex_init(void);
