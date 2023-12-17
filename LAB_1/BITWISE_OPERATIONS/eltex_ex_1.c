#include <stdio.h>
#include <stdlib.h>
#define BITS 8

int main(void)
{
    unsigned int a = 0xFFDDAABB;

    printf("value = %08X\n", a);

    printf(" 1) %08X 2) %08X 3) %08X 4) %08X\n", a >> 24, a >> 16 << 24 >> 24, a >> 8 << 24 >> 24, a << 24 >> 24);

    return EXIT_SUCCESS;
}

