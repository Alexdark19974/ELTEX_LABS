#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#define BITS 8
#define PRINT_ERROR(str) do { fprintf(stderr, "%s", str); if (errno) fprintf(stderr, "%s", strerror(errno)); puts(""); exit(EXIT_FAILURE); } while(0);

// ex.2: modify the program so you only the 3rd byte is changed, but the rest stay intact.  0x00(DD)AABB > > 0x00(00)AABB 0x00FFAABB

int main(void)
{
   unsigned int a = 0x00DDAABB, mask = 0xFFFFFFFF, byte = 0, value = 0;
   int ret = 0;

    {
        puts("                          1 2 3 4");
        printf("the signed int value = 0x%08X\n", a);

        printf("enter the number of the byte to be changed: ");

        ret = scanf("%u", &byte);

        if (ret == 0) PRINT_ERROR("error: failed to write to byte!")
        else if (ret == EOF) PRINT_ERROR("error: met EOF")
        else if (byte > 4) PRINT_ERROR("error: the chosen byte cannot less than 0!")

        puts("enter the value of the new byte: ");

        ret = scanf("%u", &value);

        if (ret == 0) PRINT_ERROR("error: failed to write to value.")
        else if (ret == EOF) PRINT_ERROR("error: met EOF")
        else if (value > UCHAR_MAX) PRINT_ERROR("error: value cannot be more than 255!")

        a = a & (mask >> (BITS * (byte)));

        value = (value << (BITS * sizeof(unsigned int) - BITS *byte)) | a;

        printf("the result is 0x%08X\n", value);
    }

    return EXIT_SUCCESS;
}

