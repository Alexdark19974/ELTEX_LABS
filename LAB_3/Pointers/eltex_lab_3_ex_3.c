#include <stdio.h>

int main(void)
{
    char a = 'A';
    char b = 'B';

    char *ptr_1 = &a;
    char **ptr_2 = &ptr_1;

    printf("ptr_1's value is %c\n", *ptr_1);

    *ptr_2 = &b;

    printf("ptr_1's value now is %c\n", *ptr_1);

   return 0;
}
