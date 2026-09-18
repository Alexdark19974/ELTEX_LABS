#ifndef MC_H
#define MC_H
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>

#define MALLOC_SIZE 128
#define STR_SIZE 512
#define SIZE 128
#define ENTER 10
#define ERROR -1
#define M_COLS 202
#define M_ROWS 2
#define TRUE 1
#define FALSE 0
#define FAIL (-1)
#define WIN_MAX 2

#define BACK_DIR_STR ".."
#define CUR_DIR_STR "."
#define UP_DIR_STR "UP--DIR"
#define NAME_NUM ".n"
#define NAME_STR "Name"
#define SIZE_STR "Size"
#define MODIFY_STR "Modify"
#define MODIFY_TIME_STR "Modify time"

typedef enum win_e {
	LEFT = 0,
	RIGHT = 1
} win_t;

typedef struct mc mc_s;

struct mc {
	WINDOW *wnd[WIN_MAX];
	WINDOW *menu_wnd[WIN_MAX];
    char data[WIN_MAX][SIZE][STR_SIZE];
    char path_way[WIN_MAX][STR_SIZE];
    int n[WIN_MAX];
    struct dirent **name_list[WIN_MAX];
    chtype ch;
    int c_y, c_x;
    bool side;
    bool refresh;
    WINDOW *(*init_wnd)(mc_s *self, win_t side);
    WINDOW *(*init_menu_wnd)(mc_s *self, win_t side);
    void (*display_wnd)(mc_s *self, win_t side, int offset);
    void (*display_line)(mc_s *self, win_t side, int offset);
    int (*get_curdir_data)(mc_s *self, win_t side);
    void (*free_curdir_data)(mc_s *self, win_t side);
};

int init_mc(mc_s *self);
void deinit_mc(mc_s *self);
int start_mc(void);
void display_wnd(mc_s *self, win_t side, int offset);
void display_line(mc_s *self, win_t side, int offset);
int get_curdir_data(mc_s *self, win_t side);
void free_curdir_data(mc_s *self, win_t side);
WINDOW *init_wnd(mc_s *self, win_t side);
WINDOW *init_menu_wnd(mc_s *self, win_t side);
int isDirectory(const char *);
int myselect(const struct dirent *entry_ptr);
void s_handler(int sig_no, siginfo_t *sig_info, void *sig_context);
#endif
