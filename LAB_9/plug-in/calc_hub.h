#ifndef _CALC_HUB_
#define _CALC_HUB_
#define MAX_LINE 50
#define BUFSIZE 1000
#define NUMBER '1'
#define MY_EOF -1
#define INFO 1
#define STATUS 2
#define CLOSE_ALL 3
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dlfcn.h>

double calc(void);

double add(double *, int *);

double sub(double *, int *);

double _div(double *, int *);

double mul(double *, int *);

int my_get_line(char *, int);

double stack_pop(double *, int *);

void push_into_stack(double *, char *, int *);

#endif
