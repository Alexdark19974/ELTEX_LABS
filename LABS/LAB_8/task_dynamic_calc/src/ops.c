#include "calculator.h"

/* NOTE: overflow and underflow are not checked...
   In case of floating values, it is not need since we merely lose precision */

double SUB_F(double val1, double val2)
{
    return val1 - val2;
}

double MUL_F(double val1, double val2)
{
    return val1 * val2;
}

double DIV_F(double val1, double val2)
{
    return val1 / val2;
}

double ADD_F(double val1, double val2)
{
    return val1 + val2;
}

int REM_F(int val1, int val2)
{
    return val1 % val2;
}

