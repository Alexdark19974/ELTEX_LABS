#include "calc_hub.h"

double stack_pop(double *stack, int *number)
{
    *number-=1;
    return stack[*number];
}
