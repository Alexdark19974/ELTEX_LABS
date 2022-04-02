#include <stdio.h>
#define MAX 100

void fill_triangle(int*, int, int);
/*
ex. 3: fill the the upper triangle with 1s, the lower with 0s
       0 0 1
       0 1 1
       1 1 1
*/
int main(void)
{
    int arr[MAX][MAX] = {0};
    int rows = 0;
    int columns = 0;

    printf("insert the size of columns and rows: ");
    scanf("%d %d", &columns, &rows);
    fill_triangle((int *)arr, columns, rows);

    return 0;
}

void fill_triangle(int *array, int len_c, int len_r)
{
    int ones = 1;
    int counter = 0;


    for (int i = len_c - 1; i >= 0; i--)
    {
        for (int j = len_r - 1; j >= 0; j--)
        {
            *((array + i) + j) = ones;

              if (j == counter)
            {
                break;
            }

        }
        counter++;
    }

    printf("the triangle is:\n");

    for (int i = 0; i != len_c; i++)
    {
        for (int j = 0; j != len_r; j++)
        {
            printf("%d ", *((array + i) + j));
        }
        puts("");
    }
}
