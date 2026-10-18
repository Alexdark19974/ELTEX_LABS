#include <stdio.h>
#include "calculator.h"

static inline void usage(void)
{
    printf("======== Reverse Polish Notation Calculator ========\n");
    printf(" all operations and numbers must be space-separated!\n");
    printf(" supported operations: + - / * %%\n");
    printf(": 102 5 +\n=107\n\n\n");
}

int main(void)
{
    usage();
    return calculator();
}