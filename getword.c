#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAXWORDSIZE 100
#define BUFSIZE 1000
#define NKEYS (sizeof(keytab) / sizeof (struct key))

struct key
{
    char *word;
    int count;
};

char buf[BUFSIZE] = {0};
int bufp = 0;

int getword (char *, int);
int binsearch (char *, struct key *, int);
int getch (void);
void ungetch (int);

int main(void)
{
    struct key keytab[] = 
    {
        {"*/", 0},
        {"/*", 0},
        {"/**/", 0},
        {"//", 0},
        {"auto", 0},
        {"break", 0},
        {"case", 0},
        {"char", 0},
        {"const", 0},
        {"continue", 0},
        {"default", 0},
        {"double", 0},
        {"enum", 0},
        {"float", 0},
        {"for", 0},
        {"int", 0},
        {"signed", 0},
        {"unsigned", 0},
        {"void", 0},
        {"volatile", 0},
        {"while", 0},
    };

    int n = 0;
    char word[MAXWORDSIZE] = {0};

    while (getword(word, MAXWORDSIZE) != EOF)
    {
        if (isalpha(word[0]) || word[0] == '/' || word[0] == '*')
        {
            if ((n = binsearch(word, keytab, NKEYS)) >= 0)
            {
                keytab[n].count++;
            }
        }
    }

    for (n = 0; n < NKEYS; n++)
    {
        if (keytab[n].count > 0)
        {
            printf ("%4d %s\n", keytab[n].count, keytab[n].word);
        }
    }

    return 0;
}

int binsearch (char *word, struct key tab[], int n)
{
    int cond = 0;
    int low = 0;
    int middle = 0;
    int high = n - 1;

    while (low <= high)
    {
        middle = (low + high) / 2;

        if ((cond = strcmp(word, tab[middle].word)) < 0)
        {
            high = middle - 1;
        }
        else if (cond > 0)
        {
            low = middle + 1;
        }
        else
        {
            return middle;
        }
    }
    return EOF;
}

int getword (char *word, int limit)
{
    int c = 0;
    char *w = word;

    while (isspace(c = getch()))
    {
        ;
    }

    if (c != EOF)
    {
        *w = c;
        w++;
    }

    if (!isalpha(c) && c != '*' && c != '/')
    {
        *w = '\0';
        return c;
    }

    for (; --limit > 0; w++)
    {
        if (!isalnum(*w = getch()) && *w != '/' && *w != '*')
        {
            ungetch (*w);
            break;
        }
    }

    *w = '\0';

    return word[0];
}

int getch(void)
{
    --bufp;
    return (bufp > 0) ? buf[bufp] : getchar();
}
void ungetch(int c)
{
    if (bufp >= BUFSIZE)
    {
        printf("ungetch: too many characters\n");
    }
    else
    {
        buf[bufp] = c;
        bufp++;
    }
}
