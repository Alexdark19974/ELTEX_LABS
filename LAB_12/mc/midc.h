#ifndef MC_H
#define MC_H

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
#include <dirent.h>

#define MALLOC_SIZE 10000
#define SIZE 10000
#define ENTER 10
#define ERROR -1
#define COLS 101
#define M_COLS 202
#define ROWS 48
#define M_ROWS 2
#define TRUE 1
#define FALSE 0

void sig_winch(int);
int isDirectory(const char *);
WINDOW *init_l_wnd(void);
WINDOW *init_r_wnd(void);
WINDOW *init_menu(void);
void mc(WINDOW *, WINDOW *, WINDOW*, int, int);
int myselect(const struct dirent *);

#endif
