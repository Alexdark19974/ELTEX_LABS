#include <stdio.h>

int main(void)
{
    int a = 270533154;
    char *p_a = (char *) &a;
    int byte = 0;
    int value = 0;

    printf("insert the byte you want to change and the number to be pushed: ");

    scanf ("%d", &byte);

    p_a += byte;

    printf("insert the the number you want to be pushed: ");

    scanf("%d", &value);

    *p_a = value;

    printf("the changed byte is %d\n", *p_a);

    int *p_p_a = &a;

    printf("the changed variable a = %d\n", *p_p_a);

    return 0;
}
