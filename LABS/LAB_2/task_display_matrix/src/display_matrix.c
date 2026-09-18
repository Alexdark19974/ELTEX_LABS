#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#define ROWMAX 100
#define COLMAX 100
#define PRINT_ERROR(str) do { \
                            fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, str); \
                            if (errno) \
                                fprintf(stderr, "[%s:%d] cause: %s\n", __func__, __LINE__, strerror(errno)); \
                         } while(0)

/* ex.1: display the matrix N x N
1 2 3
4 5 6
7 8 9
*/

/* Take 2-dimensional array and treat it as pointer to char for the simplicity */
void fill_matrix(unsigned int elements_total, unsigned int *mtrx)
{
    unsigned int counter = 0;
    unsigned int i;

    for (i = 0; i < elements_total; i++)
        mtrx[i] = ++counter;
}

/* Use the number of rows to delimit the next column */
void display_matrix(unsigned int elements_total, unsigned int rows, unsigned int *mtrx)
{
    unsigned int i;

    for (i = 0; i < elements_total; i++) {

        printf("%d ", mtrx[i]);

        if (((i + 1) % rows) == 0)
            puts("");
    }
}

int main(void)
{
    unsigned int arr[ROWMAX][COLMAX] = {0};
    unsigned int rows, cols;
    int ret;

    cols = rows = 0;

    printf("insert the number of rows and columns (e.g \"5 5\"): ");

    ret = scanf("%32u %32u", &rows, &cols);

    if (ret == 0 || ret == 1) {
        PRINT_ERROR("failed to read columns and rows!");
        goto fail;
    } else if (ret == EOF) {
        PRINT_ERROR("received EOF!");
        goto fail;
    }
    
    if ((cols >= COLMAX || rows >= ROWMAX) || (cols <= 1 || rows <= 1)) {
        PRINT_ERROR("you can't print more 100 rows or 100 columns or equal/less than 1!");
        goto fail;
    }

    printf("total number of elements of the matrix = %u\n", rows * cols);

    fill_matrix(rows * cols, *arr);

    display_matrix(rows * cols, rows, *arr);

    return EXIT_SUCCESS;

fail:
    return EXIT_FAILURE;
}

