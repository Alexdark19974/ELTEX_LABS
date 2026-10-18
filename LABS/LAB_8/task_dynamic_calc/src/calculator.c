#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include "calculator.h"
#include "stack.h"
#include "debug.h"

static size_t getLine(char *buf, size_t size);
static Op parseTok(const char *buf, double *val, Type *type);
extern int errno;
#define IS_BLANK(c) ((c) == ' ')
#define IS_TAB(c)    ((c) == '\t')
#define IS_OPER(c)   ((c) == '+' || (c) == '-' || (c) == '/' || (c) == '*' || (c) == '%')
#define PUSH() push(&stack, val)
#define POP() pop(&stack)
#define BUFSIZE 512

int calculator(void)
{
    char *tok = NULL;
    Stack stack;
    double val = 0.0, val1 = 0.0, val2 = 0.0;
    size_t len = 0;
    Op oper;
    Type type = INT;
    char buf[BUFSIZE] = {0};

    init_stack(&stack);
    PRINT_DBG("Initialized stack %p\n", (void *)&stack);

    printf(": ");

    while ((len = getLine(buf, sizeof(buf))) != 0) {

        tok = strtok(buf, " ");

        while (tok) {

            oper = parseTok(tok, &val, &type);

            switch(oper) {
                case NUM:
                    stack.PUSH()/* push(&stack, val) */;
                    break;
                case ADD:
                case MUL:
                    val1 = stack.POP();
                    val2 = stack.POP();
                    val = oper == ADD ? ADD_F(val1, val2): MUL_F(val1, val2);
                    stack.PUSH();
                    break;
                case SUB:
                case DIV:
                case REM:
                    /* 10 (val1) 5 (val2) - (sub) */
                    val2 = stack.POP();
                    if (oper == DIV && val2 == 0.0) {
                        fprintf(stderr, "devision by zero is not allowed.\n");
                        goto reset;
                    } else if (oper == REM && type == DOUBLE) {
                        fprintf(stderr, "floating-point values with remainder cannot be performed.\n");
                        goto reset;
                    }
                    val1 = stack.POP();
                    val = (oper == SUB) ? 
                            SUB_F(val1, val2) : 
                                (oper == DIV) ? DIV_F(val1, val2) : REM_F((int)(val1), (int) val2);
                    stack.PUSH();
                    break;
                case RESET:
reset:
                    while (stack.POP()!= EOS) ;
                    goto try_again;
                case FATAL_ERR:
                    goto leave_with_disgrace;
                case NONE:
                default:
                    break;
            }

            tok = strtok(NULL, " ");
        }

        if (stack.idx > 0) {
            val = stack.POP();
            snprintf(buf, sizeof(buf), "%g", val);
            printf(" = %s\n", buf);
        }

        type = INT, *buf = '\0', val = 0.0;

try_again:
        printf(": ");
    }

    return EXIT_SUCCESS;
leave_with_disgrace:
    return EXIT_FAILURE;
}

static size_t getLine(char *buf, size_t size)
{
    size_t i;

    assert(size > 0);

    assert(buf != NULL);

    for (i = 0; i < size - 1 && ((buf[i] = (char) getchar()) != EOF) && buf[i] != '\n'; ++i) ;

    if (i == size - 1) {
        PRINT_DBG("%s\n", "warning: buf is out of space: reset bufer to 0");
        buf[0] = '\0';
    }

    if (buf[i] == EOF) {
        PRINT_DBG("%s\n", "Received EOF. Terminating");
        i = 0;
    } else buf[++i] = '\0';

    return i != 0;
}

/* parseTok():
    1) parses buf;
    2) stores parsed token in val for values;
    3) stores type of token in type
    4) returns type of operation to be executeed */
static Op parseTok(const char *buf, double *val, Type *type)
{
    Op action = NONE;
    bool isNegative = false;

    assert(buf != NULL && type != NULL && val != NULL);

    if (*buf == '-' && isdigit((unsigned char)*(buf + 1))) {
        isNegative = true, ++buf;
    }

    if (isdigit((unsigned char)*buf)) {

        for (size_t i = 0, dot_cnt = 0; i < SIZE_MAX && buf[i] != '\0'; ++i) {
            if (buf[i] == '.') {
                ++dot_cnt, *type = DOUBLE;
                continue;
            }

            if (dot_cnt > 1 || (!isdigit((unsigned char)buf[i]) && buf[i] != '\n')) {
                fprintf(stderr, "error: invalid number: %s\n", buf);
                PRINT_DBG("%s\n", "there has to be one dot per numeric expression");
                PRINT_DBG("(dots = %zu)\n", dot_cnt);
                action = RESET, *type = INT;
                break;
            }
        }
        action = NUM;
    } else if (IS_OPER(*buf) && (*(buf + 1) == '\0' || *(buf + 1) == '\n'))
        action = *buf;
    else {
        if (*buf != '\n') {
            fprintf(stderr, "error: invalid character or operation");
            PRINT_DBG(": %s(char: %#X)", buf, *buf);
            fputc('\n', stderr);
        }
        action = RESET, *type = INT;
    }

    if (action == NUM) {
        errno = 0;
        *val = strtod(buf, NULL);
        if ((*val == 0 && errno) || errno) {
            PRINT_DBG("error: strtod() failed %s\n", strerror(errno));
            action = FATAL_ERR, *type = INT;
        }
        if (isNegative == true)
            *val = -(*val);
    }

    PRINT_DBG("action = %d\n", action);

    return action;
}

