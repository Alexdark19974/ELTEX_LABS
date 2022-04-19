#include "calc_hub.h"

int my_get_line (char *s, int max)
{
    char c = 0;
    int i = 0;
    char temp = 0;

    while ((c = getchar()) == ' '  && c == '\t')
    {
	;
    }

    if (c == '-')
    {
	temp = c;
	c = getchar();
    }

    if (isdigit(c))
    {
	if (temp)
	{
	    s[i] = temp;
	    i++;
	}
	s[i] = c;
	i++;

	for (; isdigit(c = getchar()); i++)
	    {
		s[i] = c;
	    }
    }

    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=')
    {
	s[i] = c;
	s[i] = '\0';
	return c;
    }

    if (c == '\n')
    {
	return c;
    }

    if (c == EOF)
    {
	return MY_EOF;
    }

    s[i] = '\0';

    return NUMBER;
}
