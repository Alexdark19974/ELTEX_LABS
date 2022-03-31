#include "calc_hub.h"

double _div(double *stack, int *counter)
{
   double digit_2 = stack_pop(stack, counter);
   double digit_1 = stack_pop(stack, counter);

   double res = digit_1 / digit_2;

    return res;
}



