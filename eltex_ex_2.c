#include <stdio.h>
#define MAX 100
/*
ex.1: display the matrix N x N
1 2 3
4 5 6
7 8 9
ex.2: display the array in reversed order:
1 2 3 4 5 > 5 4 3 2 1
ex. 3: fill the the upper triangle with 1s, the lower with 0s
0 0 1
0 1 1
1 1 1
ex.4: fill the matrix with the numbers from 1 to N^2 that looks like a snake
1  2  3  4  5
16 17 18 19 6
15 24 25 20 7
14 23 22 21 8
13 12 11 10 9 
*/
int main(void)
{
    int arr[MAX][MAX] = { 0 };
    int columns, rows;
    columns = rows = 0;

    printf("insert the number of columns and rows: ");
    scanf("%d %d", &columns, &rows);

    for (int i = 0; i != columns; i++)
    {
        static int counter = 0;

        for (int j = 0; j != rows; ++j)
        {
            printf("%d ", arr[i][j] = ++counter);
        }
        puts("");
    }

    return 0;
}