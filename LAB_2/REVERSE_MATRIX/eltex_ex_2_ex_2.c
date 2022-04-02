#include <stdio.h>
#define MAX 100

void reverse(int*, int);

int main(void)
{
    int arr[MAX] = {0};
    int length = 0;
    int counter = 0;

    printf("insert the length of the array: ");
    scanf(" %d", &length);

    for(int i = 0; i != length; i++)
    {
        arr[i] = ++counter;
    }
    reverse(arr, length);

    return 0;
}

void reverse(int *arr, int l)
{
    static int count = 0;
    static int i = 0;
    int temp = 0;

    if (count != l/2)
    {
        count++;
        reverse(arr, l);
    }
    temp = *(arr + i);
    *(arr + i) = *(((arr + l) - 1) - i);
    *(((arr + l) - 1) - i) = temp;
    i++;

    if (i == l/2)
    {
        printf("the inverted array: ");
        for (int j = 0; j != l; j++)
        {
            printf("%d ", arr[j]);
        }
	puts("");
    }
}
