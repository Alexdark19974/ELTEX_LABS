#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#define MAX 100
#define PRINT_ERROR(str) do { fprintf(stderr, "%s", str); if (errno) fprintf(stderr, "%s", strerror(errno)); puts(""); exit(EXIT_FAILURE); } while(0);

void reverse(int, int, int *);
void fill_matrix(int, int, int *);
void fill_triangle(int, int, int *);
void display_matrix(int, int, int *);

/* ex. 3: fill the the upper triangle with 1s, the lower with 0s
       0 0 1
       0 1 1
       1 1 1 */

int main(void)
{
    int arr[MAX][MAX] = {0};
    int rows, columns, counter, ret, elements_total;

    {
        columns = rows = counter = ret = elements_total = 0;

        printf("insert the number of rows and columns (e.g \"5 5\"): ");

        ret = scanf("%32d %32d", &rows, &columns);
        if (ret == 0 || ret == 1) PRINT_ERROR("failed to read rows and columns!")
        else if (ret == EOF) PRINT_ERROR("received EOF!")
        else if ((columns > MAX || rows > MAX) || (columns <= 1 || rows <= 1)) PRINT_ERROR("you can't print more 100 rows or 100 columns or equal/less than 1!")

        elements_total = rows * columns;

        printf("total number of elements in matrix = %d\n", elements_total);

        {
            puts("----------- fill matrix --------");
            fill_triangle(rows, columns, *arr);
            puts("--------------------------------");
        }

        {
            puts("------- display matrix --------");
            display_matrix(elements_total, rows, *arr);
            puts("----------------------------------------");
        }
    }

    return EXIT_SUCCESS;
}

void fill_triangle(int rows, int columns, int *mtrx)
{
    int lim = 0;

    for (int i = 0, j = 0; i < columns; i++) {
        for (j = 0; j < rows; j++) {
            if (j == rows - lim) {
                j = rows;
                break;
            }
            mtrx[j] = 1;
        }
        lim++;
        if (rows - lim < 0) // if no more rows, elements of which we can set to 1, then break
            break;
        mtrx += j; // j = number of elements to shift to the next dimension
    }
}

void display_matrix(int elements_total, int rows, int *mtrx)
{
    for (int i = 0; i < elements_total; i++) {
        printf("%d ", mtrx[i]);
        if (((i + 1) % rows) == 0)
            puts("");
    }
}

