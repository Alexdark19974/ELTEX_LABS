#ifndef COMMON_H
#define COMMON_H

#define _POSIX_C_SOURCE 200809L // necessary for fileno() as it is not a standard libc function
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#define ERR -1
#define OK 0
#define FOPEN_MODE 'f'
#define OPEN_MODE 'o'
#define UNKNOWN_MODE '?'
#define BUFSIZE 256
#define FILE_TXT "file.txt"
#endif // COMMON_H
