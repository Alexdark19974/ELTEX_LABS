#include <stdio.h>
#define BIT 8

// ex.2: modify the program so you only the 3rd byte is changed, but the rest stay intact.  0x00(DD)AABB > > 0x00(00)AABB 0x00FFAABB

int main(void)
{
    int a = 0x00DDAABB;
    int shift = 0x000000FF;
    int byte = 0;
    int value = 0;

    printf("the original value for task 2  is %x\n", a);

    printf("insert the byte that needs to be changed: ");

    scanf("%d", &byte);

    printf("instert the value that needs to be put in place of 3rd byte: ");

    scanf("%d", &value);

    printf("the input value is %x\n", value);

    a = a & (~(shift << (BIT * (byte - 1))));
    //0x000000FF << 16 > 0x00FF0000 > 0xFF00FFFF > 0x00DDAABB & 0xFF00FFFF
    // a = 0x0000AABB

    value = (value << (BIT * (byte - 1)));
    //0x0000000b << 16 > 0x000b0000

    //printf("%x\n", value);
    // 0x0000AABB |0x000b0000 > 0x000baabb
    printf("the result is %x\n", a | value);

    return 0;
}

