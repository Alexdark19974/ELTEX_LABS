#include "calc_hub.h"

static char buf[BUFSIZE];
static int bufp = 0;

int my_get_line (char *s, int max)
{
    char c = 0;
    int i = 0;
    char temp = 0;
	int getch(void);
	void ungetch(int);

    while ((c = getch()) == ' ' || c == '\t')
    {
		;
    }

    if (c == '-')
    {
		temp = c;
		c = getch();
		if (c == '\n')
    	{
    		ungetch(c);
        	return temp;
    	}
    	else if (c == ' ')
    	{
        	ungetch(c);
        	c = temp;
    	}
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

		for (; isdigit(c = getch()); i++)
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

int getch(void)
{
    return (bufp > 0) ? buf[--bufp] : getchar();
}
void ungetch(int c)
{
    if (bufp >= BUFSIZE)
    {
        printf("ungetch: too many characters\n");
    }
    else
    {
        buf[bufp++] = c;
    }
}
