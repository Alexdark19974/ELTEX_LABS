#include <stdio.h>

int main(void)
{
    int a = 270533154;
    char *p_a = (char *) &a;

    printf ("the OG number is %d\n", a);

    printf("first byte of a  = %d\n", *p_a);
    p_a++;

    printf("second byte of a = %d\n", *p_a);
    p_a++;

    printf("third byte of a = %d\n", *p_a);
    p_a++;

    printf("fourth byte of a = %d\n", *p_a);

    return 0;
}
