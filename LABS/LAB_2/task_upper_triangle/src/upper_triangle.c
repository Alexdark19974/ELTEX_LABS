#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#define ROWMAX 100
#define COLMAX 100
#define PRINT_ERROR(str) do { \
                                fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, str); \
                                if (errno) \
                                    fprintf(stderr, "[%s:%d] cause: %s\n", __func__, __LINE__, strerror(errno)); \
                         } while(0)

void fill_matrix(unsigned int, unsigned int, unsigned int *);
void fill_triangle(unsigned int, unsigned int, unsigned int *);
void display_matrix(unsigned int, unsigned int, unsigned int *);

/* ex. 3: fill the the upper triangle with 1s, the lower with 0s
       0 0 1
       0 1 1
       1 1 1 */

int main(void)
{
    unsigned int arr[ROWMAX][COLMAX] = {0};
    unsigned int rows, columns, counter, elements_total;
    int ret = 0;
    
    { /* logical block START */
        columns = rows = counter = elements_total = 0;

        printf("insert the number of rows and columns (e.g \"5 5\"): ");

        ret = scanf("%32u %32u", &rows, &columns);
    
        if (ret == 0 || ret == 1) {
            PRINT_ERROR("failed to read rows and columns!");
            goto fail;
        } else if (ret == EOF) {
            PRINT_ERROR("received EOF!");
            goto fail;
        } else if ((columns >= ROWMAX || rows >= COLMAX) || (columns <= 1 || rows <= 1)) {
            PRINT_ERROR("you can't print more 100 rows or 100 columns or equal/less than 1!");
            goto fail;
        }

        elements_total = rows * columns;

        printf("total number of elements in matrix = %u\n", elements_total);

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
    } /* logical block END */

    return EXIT_SUCCESS;
fail:
    return EXIT_FAILURE;
}

void fill_triangle(unsigned int rows, unsigned int columns, unsigned int *mtrx)
{
    unsigned int i, j, lim;

    for (i = 0, j = 0, lim = 0; i < columns; i++) {

        for (j = 0; j < rows; j++) {

            if (j == rows - lim) {
                j = rows;
                break;
            }

            mtrx[j] = 1;
        }
    
        lim++;

        if (lim == rows) // if lim == rows, then no rows, which means we can break out of the loop
            break;

        mtrx += j; // j = number of elements to shift to the next dimension
    }
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

