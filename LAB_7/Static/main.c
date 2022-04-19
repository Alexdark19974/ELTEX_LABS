#include "calc_hub.h"

int main (void)
{
    double res = 0;

    while ((res = calc()) != EOF)
    {
    	printf(" = %lf;\n", res);
    }

    return 0;
}
