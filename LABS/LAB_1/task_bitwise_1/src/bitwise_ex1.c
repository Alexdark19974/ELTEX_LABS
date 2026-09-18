#include <stdio.h>
#include <stdlib.h>

/* Display every byte of type unsigned int */

int main(void)
{
    unsigned int a = 0xFFDDAABB;

    printf("value = %#X\n", a);

    printf(
            "1) %#X\n"
            "2) %#X\n"
            "3) %#X\n"
            "4) %#X\n", 
            a >> 24,
            a >> 16 << 24 >> 24, // taking advantage of equal precedence and left assoociativity
            a >> 8 << 24 >> 24, // (((a >> 8) << 24) >> 24)
            a << 24 >> 24 
        );

    return EXIT_SUCCESS;
}

