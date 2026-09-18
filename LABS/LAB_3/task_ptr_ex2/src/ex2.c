#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define PRINT_ERROR(str) do { \
                            fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, str); \
                            if (errno) \
                                fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, strerror(errno)); \
                            } while(0)

/* Change one of 4 bytes of unsigned int variable */

void display_endianness(void)
{
    unsigned int val = 0x1234;
    printf("The arch is ");
    puts(*((unsigned char *)&val) == 0x12 ? "big-endian" : "little-endian");
}

int main(void)
{
    unsigned char *p_a = NULL;
    unsigned int a = 0x12345678;
    int ret = 0;
    unsigned char byte = 0, value = 0;

    p_a = (unsigned char *) &a;

    printf("Old value= 1 2 3 4\n");
    printf("\t%#X\n", a);

    printf("pick the byte: ");

    ret = scanf ("%hhu", &byte);

    if (ret <= 0) {
        PRINT_ERROR("Failed to write to var!");
        goto fail;
    } else if (byte > 4 || byte < 1) {
        PRINT_ERROR("the chosen byte cannot be more than 4 and less than 1!");
        goto fail;
    }

    printf("enter the new value [0 - 255]: ");

    ret = scanf("%hhu", &value);

    if (ret <= 0) {
        PRINT_ERROR("Failed to write to var!");
        goto fail;
    }

    *(p_a + sizeof(unsigned int) - byte) = value;

    printf("updated variable a = %#X\n", a);

    return EXIT_SUCCESS;

fail:
    return EXIT_FAILURE;
}
