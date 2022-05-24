#ifndef NANO_H

#define NANO_H
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
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

#define MALLOC_SIZE 1000

void input(char **);

void sig_winch(int signo);

#endif
