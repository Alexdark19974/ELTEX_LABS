#include <stdio.h>

int main(void)
{
    int a = 270533154;
    char *p_a = &a;
    int byte = 0;
    int value = 0;
    
    printf ("the OG number is %d\n", a);
    printf("first byte of a  = %d\n", *p_a);
    p_a++;
    printf("second byte of a = %d\n", *p_a);
    p_a++;
    printf("third byte of a = %d\n", *p_a);
    p_a++;
    printf("fourth byte of a = %d\n", *p_a);

    p_a = &a;

    printf("insert the byte you want to change and the number to be pushed: ");
    scanf ("%d", &byte);
    p_a += byte;

    printf("insert the the number you want to be pushed: ");
    scanf("%d", &value);
    *p_a = value;

    printf("the changed byte is %d\n", *p_a);

    int *p_p_a = &a;

    printf("the changed variable a = %d", *p_p_a);

    return 0;
}