#ifndef COMMON_H
#define COMMON_H

#define _POSIX_C_SOURCE 200809L // necessary for fileno() as it is not a standard libc function
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <curses.h>
#define MAX_LEN 11
#define MAX_NAME_LEN 15
#define RIGHT_PASSWORD "12345678"

void sig_handler(int signum);

#endif // COMMON_H

