#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#define PRINT_ERROR(str) do { fprintf(stderr, "%s", str); if (errno) fprintf(stderr, "%s", strerror(errno)); puts(""); exit(EXIT_FAILURE); } while(0);
#define MAX 100 // hard-coded limit
#if defined(ARRAY_SOLUTION)
void snake_matrix(int rows, int columns, int [rows][columns]);
void display_matrix(int rows, int columns, int [rows][columns]);
#else
void snake_matrix(int, int, int *);
void display_matrix(int, int, int *);
#endif

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
#ifndef ARRAY_SOLUTION
        snake_matrix(rows, columns, *arr);
        display_matrix(columns, elements_total, *arr);
#else
        snake_matrix(rows, columns, arr);
        display_matrix(rows, columns, arr);
#endif
    }

    return EXIT_SUCCESS;
}

#ifndef ARRAY_SOLUTION
void snake_matrix(int rows, int columns, int *mtrx)
{
    int counter = 0, depth = 0, rounds = (columns / 2) + (columns % 2);

    for (int i = 0; i < rounds; i++) {
        for (int row = columns * depth, column = depth;
                (counter < rows * columns) && column < columns - depth;
                column++) {
            mtrx[row + column] = ++counter;
        }
#if 0
        if (depth == 2)
            return;
#endif
        for (int row = columns * (depth + 1), column = columns - depth - 1;
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
        for (   int row = rows * columns - (columns * (depth + 1)), column = columns - 2 - depth;
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

        for ( int row = rows * columns - (columns * (depth +1)), column = depth;
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
void snake_matrix(int rows, int columns, int mtrx[rows][columns]) // VLA
{
    int counter = 0, depth = 0;
    int rounds = (columns / 2) + (columns % 2);

    for (int i = 0; i < rounds; i++) {

        for (int row = depth, column = depth; (counter < rows * columns) && column < columns - depth; column++) {
            mtrx[row][column] = ++counter;
        }

        for (int row = depth + 1, column = columns - depth - 1; (counter < rows * columns) && row < rows - depth; row++) {
            mtrx[row][column] = ++counter;
        }

        for (int row = rows - depth - 1, column = columns - depth - 2; (counter < rows * columns) && column > depth; column--) {
            mtrx[row][column] = ++counter;
        }

        for (int row = rows - depth - 1, column = depth; (counter < rows * columns) && row > depth; row--) {
            mtrx[row][column] = ++counter;
        }
        depth++;
    }
}
#endif

#ifndef ARRAY_SOLUTION
void display_matrix(int columns, int elements_total, int *mtrx)
{
    for (int i = 0; i < elements_total; i++) {
        printf("%d ", mtrx[i]);
        if (((i + 1) % columns) == 0)
            puts("");
    }
}
#else
void display_matrix(int rows, int columns, int mtrx[rows][columns])
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%d ", mtrx[i][j]);
        }
        puts("");
    }
}
#endif