#ifndef _CALC_HUB_
#define _CALC_HUB_
#define MAX_LINE 50
#define MALLOC_BUF 100
#define HANDLER_SIZE 50
#define PATH_ARR_SIZE 50
#define BUFSIZE 1000
#define ARR_SIZE 1000
#define CPY_LIMIT 100
#define NUMBER '1'
#define MY_EOF -1
#define ASCII 48
#define DIV 1
#define ADD 2
#define SUB 3
#define MUL 4
#define GET_LINE 5
#define STACK 6
#define CALC 7
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>

double my_calc(void);

double add(double *, int *, double (*)(double *, int *));

double sub(double *, int *, double (*)(double *, int *));

double _div(double *, int *, double (*)(double *, int *));

double mul(double *, int *, double (*)(double *, int *));

int my_get_line(char *, int);

double stack_pop(double *, int *);

void push_into_stack(double *, char *, int *);

typedef int (*func_line_ptr) (char *, int);

typedef double (*func_ptr)(void);

typedef double (*func_add_ptr)(double *, int *, double (*)(double *, int *));

typedef double (*func_sub_ptr)(double *, int *, double (*)(double *, int *));

typedef double (*func_div_ptr)(double *, int *, double (*)(double *, int *));

typedef double (*func_mul_ptr)(double *, int *, double (*)(double *, int *));

typedef double (*func_stack_ptr)(double *, char *, int *);

typedef double (*func_pop_ptr)(double *, int *);
#endif
