#include <stdio.h>
#define MAX 100

void snake(int *, int, int);
/*
ex.4: fill the matrix with the numbers from 1 to N^2 that looks like a snake
1  2  3  4  5
16 17 18 19 6
15 24 25 20 7
14 23 22 21 8
13 12 11 10 9 
*/

int main(void)
{
    int arr[MAX][MAX] = {0};
    int columns = 0;
    int rows = 0;
    int counter = 0;
    int upper_shift = 0;

    printf("insert the size of columns and rows: ");
    
    scanf("%d %d", &columns, &rows);

    int cycles = columns / 2;

    for (int count = 0; count <= cycles && count != columns * rows; count++)
    {
        for (int i = 0 + upper_shift; i != columns - upper_shift && count != columns * rows; i++)
        {
            for (int j = 0 + upper_shift; j != rows - upper_shift && count != columns * rows; j++)
            {
                arr[i][j] = ++counter;
            }
            break;
        }
        for (int i = 1 + upper_shift; i != columns - upper_shift && count != columns * rows; i++)
        {
            for (int j = rows - 1 - upper_shift; j != rows - upper_shift && count != columns * rows;)
            {
                arr[i][j] = ++counter;
                i++;
                
                if (i == columns - upper_shift)
                {
                    break;
                }
            }
            break;
        } 
        for (int i = columns - 1 - upper_shift; i != columns - upper_shift && count != columns * rows; i++)
        {
            for (int j = rows - 2 - upper_shift; j >= upper_shift && count != columns * rows;)
            {
                arr[i][j] = ++counter;
                j--;
            }
            break;
        }
        for (int i = columns - 2 - upper_shift; i > 0 + upper_shift && count != columns * rows;)
        {
            for (int j = upper_shift; j != columns - upper_shift && count != columns * rows;)
            {
                arr[i][j] = ++counter;
                i--;
                
                if (i == 0 + upper_shift)
                {
                    break;
                }
            }
            break;
        }
        upper_shift++;
    }
    
    for (int i = 0; i != columns; i++)
    {
        for (int j = 0; j != rows; j++)
        {
            
              (arr[i][j] <= rows) ? 
              printf(" %d  ", arr[i][j]) 
            : printf(" %d ", arr[i][j]);
        }
        puts("");
    }

     return 0;
}