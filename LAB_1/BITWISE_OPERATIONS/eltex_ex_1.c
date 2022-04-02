#include <stdio.h>
#define BIT 8

// ex.1: display each byte: BB, AA, DD, 00.

int main(void)
{
    int a = 0x00DDAABB;
    int shift = 0x000000FF;

    printf("the original hex is %x\n", a);

    int res_1 = a & shift;
    int res_2 = a & (shift << (BIT * 1));
    //shifting 1 byte (8 bit) to the left > 0xAA00
    int res_3 = a & (shift << (BIT * 2));
    //shifting 2 bytes (16 bits) to the left >0xDD0000
    int res_4 = a & (shift << (BIT * 3));
    // shifting 3 bytes (24 bits) to the left 0x0

    printf(" 1) %x\n 2) %x\n 3) %x\n 4) %x\n", res_1, res_2, res_3, res_4);

    return 0;
}
