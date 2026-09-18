#ifndef BASH_H

#define BASH_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#define MAX_ARGS_LIST 50
#define MAX_COMMANDS 50
#define MAX_PATTERNS 50
#define MAX_BUF 50
#define MAX_DIR 100
#define MAX_GARBAGE_COLLECTOR 10000
#define handle_error_en(en, msg) \
     do { errno = en; perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
     do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

void bash(void);
int string_parser(char *, char ***, char**, char **, int *);
void wait_for_child(int);

#endif
