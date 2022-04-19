#include "calc_hub.h"

double calc (void)
{
    char buf_for_calc[MAX_LINE] = {0};
    double res = 0;
    double stack[MAX_LINE] = {0.0};
    int counter = 0;
    char token = 0;

    printf ("Insert data to be calculated: ");

    for (;(token = my_get_line(buf_for_calc, MAX_LINE)) != '\n';)
	{
	    switch(token)
	    {
		case MY_EOF:
		{
		    return EOF;
		}
		case NUMBER:
		{
		    push_into_stack(stack, buf_for_calc, &counter);
		    break;
		}
		case '+':
		{
		    res = add(stack, &counter);
		    break;
		}
		case '-':
		{
                    res = sub(stack, &counter);
                    break;
		}
		case '*':
		{
		    res = mul(stack, &counter);
                    break;
		}
		case '/':
		{
		    res = _div(stack, &counter);
                    break;
		}
		case '=':
		{
		    getchar();
		    return res;
		}
		default:
		{
		    printf ("error: unknown command\n");
		    exit (-1);
		}
	    }
	}

	return res;
}
