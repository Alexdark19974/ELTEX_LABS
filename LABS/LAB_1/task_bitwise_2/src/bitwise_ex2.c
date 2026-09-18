#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#define BITS 8
#define RIGHT_SHIFT(val, pos) ((val) >> (pos))
#define LEFT_SHIFT(val, pos) ((val) << (pos))
#define PRINT_ERROR(str) do { \
                                 fprintf(stderr, "%s\n", str); \
                                 if (errno) \
                                    fprintf(stderr, "cause: %s\n", strerror(errno)); \
                              } while(0)
/*
   modify the program so you the 3rd byte is changed, but the rest stay intact.  0x00(DD)AABB > > 0x00(00)AABB 0x00FFAABB
*/

void display_endianness(void);

int modify_byte(void);

int main(void)
{
    return modify_byte();
}

int modify_byte(void)
{
    unsigned char *pa = NULL;
    unsigned int a = 0xBBFFDDAA, mask = 0xFFFFFFFF, byte = 0, value = 0;
    int ret = 0;

    display_endianness();

    {
        printf("usage:\n\t 1  2   3  4\n\t BB FF DD AA\n");
        printf("unsigned int value = %#X\n", a);
        printf("mask=%#08X\n", mask);

        printf("enter the number of the byte to be changed: ");

        pa = (unsigned char *) &a;

        ret = scanf("%u", &byte);

        if (ret == 0) {
            PRINT_ERROR("error: failed to write to byte!");
            goto fail;
        } else if (ret == EOF) {
            PRINT_ERROR("error: met EOF");
            goto fail;
        } else if (byte < 1 || byte > 4) {
            PRINT_ERROR("error: the chosen byte cannot less than 1 and more than 4!");
            goto fail;
        }

        printf("You have picked byte %u\n", byte);
        printf("Prev value = %#X\n", *(pa + sizeof(unsigned int) - byte));

        puts("enter the value of the new byte: ");

        ret = scanf("%u", &value);

        printf("New value = %#X\n", value);

        if (ret == 0) {
            PRINT_ERROR("error: failed to write to value!");
            goto fail;
        } else if (ret == EOF) {
            PRINT_ERROR("error: met EOF");
            goto fail;
        } else if (value > UCHAR_MAX) {
            PRINT_ERROR("error: value cannot be more than 255!");
            goto fail;
        }

        a &= ~(LEFT_SHIFT(RIGHT_SHIFT(mask, sizeof(unsigned int) * BITS - BITS), (sizeof(unsigned int) - byte) * BITS));
        a |= LEFT_SHIFT(value, (sizeof(unsigned int) - byte) * BITS);
        
        printf("a = %#08X\n", a);
    }

    return EXIT_SUCCESS;

fail:
    return EXIT_FAILURE;
}

void display_endianness(void)
{
    unsigned int val = 0x1234;
    unsigned char *pval = (unsigned char *) &val;

    printf("1st byte=%#X\n", *pval);

    if (*pval == 0x12)
        puts("arch is big-endian!");
    else
        puts("arch is little-endian!");
}

