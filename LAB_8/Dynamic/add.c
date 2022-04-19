#include "calc_hub.h"


int a = 10;
double add (double *stack, int *counter)
{
    int b = 10;
    b = 10 + a;

    double digit_2 = stack_pop(stack, counter);
    double digit_1 = stack_pop(stack, counter);

    double res = digit_1 + digit_2;

    return res;
}
