#include "calc_hub.h"

int const id = ADD;

double add (double *stack, int *counter, double (*pop_stack)(double *, int *))
{
    double digit_2 = pop_stack(stack, counter);
    double digit_1 = pop_stack(stack, counter);

    double res = digit_1 + digit_2;

    return res;
}
