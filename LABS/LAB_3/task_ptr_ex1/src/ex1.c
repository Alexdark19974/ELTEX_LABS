#include <stdio.h>

/* Display every byte of unsigned int using a pointer */

void display_endianness(void)
{
    unsigned int val = 0x1234;
    printf("The arch is ");
    puts(*((unsigned char *)&val) == 0x12 ? "big-endian" : "little-endian");
}

int main(void)
{
    unsigned int a = 0x12345678;
    unsigned char *p_a = (unsigned char *) &a;

    display_endianness();

    printf("Original val is %#X\n", a);

    printf(
            "1 - %#X, "
            "2 - %#X, "
            "3 - %#X, "
            "4 - %#X\n", *p_a, *(p_a + 1), *(p_a + 2), *(p_a + 3)
        );

    return 0;
}
