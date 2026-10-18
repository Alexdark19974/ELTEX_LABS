#ifndef NANO_H
#define NANO_H
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <curses.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>

#define FAIL (-1)
#define ERR_BUFSIZ 256
#define BEG_Y 1
#define BEG_X 1

#ifndef MALLOC_SIZE
#define MALLOC_SIZE 1000
#endif

#ifndef FILE_NAME
#define FILE_NAME 128
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef STR_SIZ
#define STR_SIZ 128
#endif

#define DISPLAY_EDITOR_NAME "text editor 0.0.1"
#define DISPLAY_NEW_BUFFER "new buffer"
#define DISPLAY_MODIFIED "Modified      "
#define DISPLAY_PRESS_ANY_KEY "presss ANY key to exit"

typedef enum WND_T {
    STDSCR = 0,
    MENUSCR = 1,
} WND_E;

typedef struct nano nano_s;

int init_nano(nano_s *self);
void deinit_nano(nano_s *self);
#ifdef DEBUG
void display_xy(nano_s *self);
#endif
bool repos_curs(nano_s *self);
void redraw(nano_s *self, WND_E wnd_type);

struct nano {
    WINDOW *menu_wnd;
    int m_y, m_x;
    int max_x;
    int c_y, c_x;
    char **lin_buf;
    char err_buf[ERR_BUFSIZ];
    chtype ch, subch;
    void (*redraw)(nano_s *self, WND_E wnd_type);
#ifdef DEBUG
    void (*display_xy)(nano_s *self);
#endif
};

#define MENU_BORDER (nano.m_y - 5)
#define LEFT_BORDER (BEG_X)
#define UPPER_BORDER (BEG_Y)
#define RIGHT_BORDER (nano.m_x - 1)
#define IS_EMPTY(x) ((x) == '\0')
#define IS_NEWLINE(x) ((x) == '\n')
#define NEWLINE '\n'

int nano_editor(int argc, char **argv);
void s_handler(int sig_no, siginfo_t *sig_info, void *sig_context);

#endif /* NANO_H */
