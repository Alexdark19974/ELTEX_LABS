#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#define PRINT_ERROR(str) do { \
                            fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, str); \
                            if (errno) \
                                fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, strerror(errno)); \
                            } while(0)
/* hard-coded limit: no possibility of overflow or underflow */
#define ROWMAX 100
#define COLMAX 100
#ifdef ARRAY_SOLUTION
void snake_matrix(unsigned int rows, unsigned int columns, unsigned int [rows][columns]);
void display_matrix(unsigned int rows, unsigned int columns, unsigned int [rows][columns]);
#else
void snake_matrix(unsigned int, unsigned int, unsigned int *);
void display_matrix(unsigned int, unsigned int, unsigned int *);
#endif

int main(void)
{
    unsigned int arr[ROWMAX][COLMAX] = {0};
    unsigned int rows, columns, counter, elements_total;
    int ret = 0;

#ifdef ARRAY_SOLUTION
    fprintf(stderr, "[%s:%d] '%s': ARRAY SOLUTION\n", __func__, __LINE__, "matrix_as_snake");
#endif

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

        printf("total number of elements in matrix = %u\n", elements_total);

#ifndef ARRAY_SOLUTION
        snake_matrix(rows, columns, *arr);
        display_matrix(columns, elements_total, *arr);
#else
        snake_matrix(rows, columns, arr);
        display_matrix(rows, columns, arr);
#endif
    }

    return EXIT_SUCCESS;
fail:
    return EXIT_FAILURE;
}

#ifndef ARRAY_SOLUTION
void snake_matrix(unsigned int rows, unsigned int columns, unsigned int *mtrx)
{
    unsigned int counter = 0, depth = 0, rounds = (columns / 2) + (columns % 2), i, row, column;

    for (i = 0; i < rounds; i++) {
        for (row = columns * depth, column = depth;
                (counter < rows * columns) && column < columns - depth;
                column++) {
            mtrx[row + column] = ++counter;
        }
#if 0
        if (depth == 2)
            return;
#endif
        for (row = columns * (depth + 1), column = columns - depth - 1;
                (counter < rows * columns) && row < (rows * columns - (columns * depth));
                row += columns) {
            mtrx[row + column] = ++counter;
  //          printf("column = %d, row = %d\n", column, row);
#if 0
        if (depth == 2)
            return;
#endif
        }
#if 0
        if (depth == 2)
            return;
#endif
        for (row = rows * columns - (columns * (depth + 1)), column = columns - 2 - depth;
                (counter < rows * columns) && column > depth;
                column--) {
            mtrx[row + column] = ++counter;
          //  printf("column = %d, row = %d\n", column, row);
#if 0
        if (depth == 1)
            return;
#endif
        }
#if 0
        if (depth == 1)
            return;
#endif

        for (row = rows * columns - (columns * (depth +1)), column = depth;
                (counter < rows * columns) && row > depth * columns;
                row -= columns) {
            mtrx[row + column] = ++counter;
           // printf("\n---------\nrow + column = %d, column = %d, row = %d\n", row + column, column, row);
#if 0
        if (depth == 1)
            return;
#endif
        }
#if 0
        if (depth == 1)
            return;
#endif
//        break;
        depth++; // used to go "deeper" in the matrix
    }
}
#else
/* This is by far the most human-readable solution */
void snake_matrix(unsigned int rows, unsigned int columns, unsigned int mtrx[rows][columns]) // VLA
{
    unsigned counter = 0, depth = 0, i, row, column;
    unsigned int rounds = (columns / 2) + (columns % 2);

    for (i = 0; i < rounds; i++) {

        for (row = depth, column = depth; (counter < rows * columns) && column < columns - depth; column++) {
            mtrx[row][column] = ++counter;
        }

        for (row = depth + 1, column = columns - depth - 1; (counter < rows * columns) && row < rows - depth; row++) {
            mtrx[row][column] = ++counter;
        }

        for (row = rows - depth - 1, column = columns - depth - 2; (counter < rows * columns) && column > depth; column--) {
            mtrx[row][column] = ++counter;
        }

        for (row = rows - depth - 1, column = depth; (counter < rows * columns) && row > depth; row--) {
            mtrx[row][column] = ++counter;
        }

        depth++;
    }
}
#endif

#ifndef ARRAY_SOLUTION
void display_matrix(unsigned int columns, unsigned int elements_total, unsigned int *mtrx)
{
    unsigned int i;

    for (i = 0; i < elements_total; i++) {

        printf("%d ", mtrx[i]);

        if (((i + 1) % columns) == 0)
            puts("");
    }
}
#else
void display_matrix(unsigned int rows, unsigned int columns, unsigned int mtrx[rows][columns])
{
    unsigned int i, j;

    for (i = 0; i < rows; i++) {

        for (j = 0; j < columns; j++)
            printf("%d ", mtrx[i][j]);

        puts("");
    }
}
#endif
