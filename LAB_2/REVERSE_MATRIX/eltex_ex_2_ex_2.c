#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#define MAX 100
#define PRINT_ERROR(str) do { fprintf(stderr, "%s", str); if (errno) fprintf(stderr, "%s", strerror(errno)); puts(""); exit(EXIT_FAILURE); } while(0);

void reverse(int, int, int *);
void fill_matrix(int, int, int *);
void display_matrix(int, int, int *);

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
            fill_matrix(elements_total, rows, *arr);
            puts("--------------------------------");
        }

        {
            puts("------- display reversed matrix --------");
            reverse(elements_total, 0, *arr);
            display_matrix(elements_total, rows, *arr);
            puts("----------------------------------------");
        }
    }

    return EXIT_SUCCESS;
}

void fill_matrix(int elements_total, int rows, int *mtrx)
{
    int counter = 0;

    for (int i = 0; i < elements_total; i++)
        mtrx[i] = ++counter;
}

void display_matrix(int elements_total, int rows, int *mtrx)
{
    for (int i = 0; i < elements_total; i++) {
        printf("%d ", mtrx[i]);
        if (((i + 1) % rows) == 0)
            puts("");
    }
}

void reverse(int elements_total, int counter, int *mtrx)
{
    int temp = 0;

    /*  0  1  2  3  4  5  6  7  8  9
     * [1][2][3][4][5] | [6][7][8][9][10]
     *      subarr 1   <-->     subarr 2
     * 10 - 1 = 91
     * */
    if (counter < (elements_total / 2 - 1)) // -1 to account for 0 to S -1 range of an array; S - size (e.g. 10)
        reverse(elements_total, counter + 1, mtrx);

    temp = *(mtrx + counter); // temp = mtrx[counter];
    *(mtrx + counter) = *(mtrx + elements_total - counter - 1); // mtrx[counter] = mtrx[elements_total - counter - 1];
    *(mtrx + elements_total - counter - 1) = temp; // mtrx[elements_total - counter -1] = temp;
}

