#include "calc_hub.h"

void push_into_stack(double *stack, char *buf, int *number)
{
    int temp = *number;
    double digit = atof(buf);
    stack[temp] = digit;
    *number = *number + 1;
}
