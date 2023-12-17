#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#define PRINT_ERROR(str) do { fprintf(stderr, "%s", str); if (errno) fprintf(stderr, "%s", strerror(errno)); puts(""); exit(EXIT_FAILURE); } while(0);
#define MAX 100 // hard-coded limit
#if defined(ARRAY_SOLUTION)
void snake_matrix(int, int, int [][]);
#else
void snake_matrix(int, int, int *);
#endif
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

        snake_matrix(rows, columns, *arr);

        display_matrix(elements_total, rows, *arr);
    }

    return EXIT_SUCCESS;
}

#ifndef ARRAY_SOLUTION
void snake_matrix(int rows, int columns, int *mtrx)
{
    int counter = 0;
    int rounds = (columns / 2) + (columns % 2);
    int l_to_r_upper = 0, l_border_down = 0;
    int r_to_l_lower = 0, r_border_up = 0;
    int depth = 0, offset = 0;

    for (int i = 0; i < rounds; i++) {

        offset+=rows; // how farther left we need to depth in one-dimensional array

        for (l_to_r_upper = depth; (counter < rows * columns) && l_to_r_upper < rows - depth; l_to_r_upper++) {
            mtrx[l_to_r_upper + (rows * depth)] = ++counter;
        }
#if 0
        if (depth == 2)
            return;
#endif

        for (l_border_down = l_to_r_upper + offset - 1; (counter < rows * columns) && l_border_down < rows * (columns - depth); l_border_down += rows /* Move down at the end of each row */) {
            mtrx[l_border_down] = ++counter;
        }
#if 0 
        if (depth == 0)
            return;
#endif
        /* l_to_r_upper - 1 -depth is a counter for r_to_l_lower */
        for (r_to_l_lower = l_border_down - rows - 1; (counter < rows * columns) && l_to_r_upper - 1 - depth > 0; r_to_l_lower-- , l_to_r_upper--) {
            mtrx[r_to_l_lower] = ++counter;
        }
#if 0
        if (depth == 0)
            return;
#endif

        for (r_border_up = r_to_l_lower - rows + 1; (counter < rows * columns) && r_border_up > depth * rows + depth; r_border_up -= rows) {
            mtrx[r_border_up] = ++counter;
        }
#if 0
        if (depth)
            return;
#endif
//        break;
        depth++; // used to go "deeper" in the matrix
    }
}
#else
void snake_matrix(int rows, int columns, int mtrx[rows][columns])
{
    int counter = 0;
    int rounds = (columns / 2) + (columns % 2);
    int left = 0, left_down = 0;
    int right = 0, right_up = 0;
    int depth = 0, offset = 0;

    for (int i = 0; i < rounds; i++) {

        offset+=rows; // how farther left we need to depth in one-dimensional array

        for (left = depth; (counter < rows * columns) && left < rows - depth; left++) {
        }

        for (left_down = left + offset - 1; (counter < rows * columns) && left_down < rows * (columns - depth); left_down += rows) {
            mtrx[left_down][left] = ++counter;
        }

        for (right = left_down - rows - 1; (counter < rows * columns) && left - 1 - depth > 0 ; right--, left--) {
            mtrx[right] = ++counter;
        }

        for (right_up = right - rows + 1; (counter < rows * columns) && right_up > depth * rows + depth; right_up -= rows) {
            mtrx[right_up] = ++counter;
        }

        depth++;
    }
}
#endif

void display_matrix(int elements_total, int rows, int *mtrx)
{
    for (int i = 0; i < elements_total; i++) {
        printf("%d ", mtrx[i]);
        if (((i + 1) % rows) == 0)
            puts("");
    }
}

