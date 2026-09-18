#include <stdio.h>
#include <stdlib.h>

/* Change value of pointer using a double pointer */

int main(void)
{
    unsigned char a = 0x12;
    unsigned char b = 0x74;
    unsigned char *ptr_1 = &a;
    unsigned char **ptr_2 = &ptr_1;

    printf("ptr_1's value is %#X, a is %#X\n", *ptr_1, a);

    *ptr_2 = &b;

    printf("ptr_1's value now is %#X, a is %#X\n", *ptr_1, a);

    return EXIT_SUCCESS;
}
