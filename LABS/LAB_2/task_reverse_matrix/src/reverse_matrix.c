#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#define ROWMAX 100
#define COLMAX 100
#define PRINT_ERROR(str) do { \
                                fprintf(stderr, "[%s:%d] %s", __func__, __LINE__, str); \
                                if (errno) \
                                    fprintf(stderr, "[%s:%d] cause: %s\n", __func__, __LINE__, strerror(errno)); \
                         } while(0)

void reverse(unsigned int, unsigned int, unsigned int *);
void fill_matrix(unsigned int, unsigned int *);
void display_matrix(unsigned int, unsigned int, unsigned int *);

int main(void)
{
    unsigned int arr[ROWMAX][COLMAX] = {0};
    unsigned int rows, columns, counter, elements_total;
    int ret = 0;

    {
        columns = rows = counter = elements_total = 0;

        printf("insert the number of rows and columns (e.g \"5 5\"): ");

        ret = scanf("%32u %32u", &rows, &columns);

        if (ret == 0 || ret == 1) {
            PRINT_ERROR("failed to read rows and columns!");
            goto fail;
        } else if (ret == EOF) {
            PRINT_ERROR("received EOF!");
            goto fail;
        } else if ((columns >= COLMAX || rows >= ROWMAX) || (columns <= 1 || rows <= 1)) {
            PRINT_ERROR("you can't print more 100 rows or 100 columns or equal/less than 1!");
            goto fail;
        }

        elements_total = rows * columns;

        printf("total number of elements in matrix = %d\n", elements_total);

        {
            puts("----------- fill matrix --------");
            fill_matrix(elements_total, *arr);
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
fail:
    return EXIT_FAILURE;
}

void fill_matrix(unsigned int elements_total, unsigned int *mtrx)
{
    unsigned int counter = 0, i;

    for (i = 0; i < elements_total; i++)
        mtrx[i] = ++counter;
}

void display_matrix(unsigned int elements_total, unsigned int rows, unsigned int *mtrx)
{
    unsigned int i;

    for (i = 0; i < elements_total; i++) {

        printf("%d ", mtrx[i]);

        if (((i + 1) % rows) == 0)
            puts("");
    }
}

void reverse(unsigned int elements_total, unsigned int counter, unsigned int *mtrx)
{
    unsigned int temp = 0;

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

