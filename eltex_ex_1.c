#include <stdio.h>
#define BIT 8
// ex.1: display each byte: BB, AA, DD, 00.
// ex.2: modify the program so you only the 3rd byte is changed, but the rest stay intact.  0x00(DD)AABB > > 0x00(00)AABB 0x00FFAABB
int main(void)
{
    int a = 0x00DDAABB;
    int shift = 0x000000FF;
    int byte = 0;
    int value = 0;
    printf("the original hex is %x\n", a);
    int res_1 = a & shift;
    int res_2 = a & (shift << (BIT * 1));
    //shifting 1 byte (8 bit) to the left > 0xAA00
    int res_3 = a & (shift << (BIT * 2));
    //shifting 2 bytes (16 bits) to the left >0xDD0000
    int res_4 = a & (shift << (BIT * 3)); 
    // shifting 3 bytes (24 bits) to the left 0x0

    printf(" 1) %x\n 2) %x\n 3) %x\n 4) %x\n", res_1, res_2, res_3, res_4);

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