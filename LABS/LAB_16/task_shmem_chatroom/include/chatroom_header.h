#ifndef CHATROOM_HEADER_H
#define CHATROOM_HEADER_H

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <errno.h>
#include <curses.h>
#include <pthread.h>
#define SEGSIZE 4096
#define MAX_USR 100
#define TRUE 1
#define FALSE 0
#define error_check(errn, msg) \
        do {if (-1 == errn) {perror(msg); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);} } while(0)
#define error_check_ptr(msg) \
        do { perror(msg); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);} while(0)

typedef struct Client_server
{
    int shm_fd_1;
    sem_t *sem_ptr_1;
    sem_t *sem_ptr_2;
    char *data_ptr_1;
    WINDOW *chit_chat_wnd_ptr;
    WINDOW *user_list_wnd_ptr;
    WINDOW *tab_wnd_ptr;
    WINDOW *update_wnd_ptr;
} Client_server;

typedef struct Garbage_collector
{
    struct Client_server *ptr;
    int shm_fd_1;
    char *shm_name_1_ptr;
    char *shm_ipc_ptr_1;
    char *sem_name_ptr_1;
    char *sem_name_ptr_2;
    sem_t *sem_ptr_1;
    sem_t *sem_ptr_2;
    char *data_ptr_1;
} Garbage_collector;

void chatroom_server_init(void);
void sig_winch(int);
WINDOW *init_tab_wnd(void);
WINDOW *init_chit_chat_wnd(void);
WINDOW *init_user_list_wnd(void);
WINDOW *init_update_wnd(void);
int shm_mem_init(char *);
void refresh_chat_room(struct Client_server *);
void delete_windows(WINDOW *, WINDOW *, WINDOW *, WINDOW*);
void server_control_room(struct Client_server *);
void cleanup(struct Garbage_collector *);
void clean_string(char *);

#endif