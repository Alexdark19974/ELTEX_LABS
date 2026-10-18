#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#define MATRIX_NUM 3
#define MATRIX_A_B 3
#define MATRIX_A 0
#define MATRIX_B 1
#define MATRIX_C 2
#define MAX_NUM 6
#define ERR -2
#define MAX_THREAD_NUM 2
#define BUF_SIZ 256
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)
#define handle_error_msg(fmt, ...) do { \
                            fprintf(stderr, fmt, __VA_ARGS__); \
                        } while(0)

typedef struct thread_data_s {
    int **matrices[MATRIX_NUM];
    int rows[MATRIX_NUM];
    int cols[MATRIX_NUM];
} thread_data_t;

static thread_data_t matrices_data;
void display_matrix(int **matrix, int rows, int cols);
int matrix_start(char **argv);

void *multiply_func(void *args)
{
    int idx, i, j, cum;

    /* do not check becasue we use ptr as increment storage */
    /* ptr = 0 is valid */
#if 0
    if (!args)
        return NULL;
#endif

    idx = (int)(uintptr_t)args;

    for (i = 0; i < matrices_data.cols[MATRIX_C]; i++) {
        cum = 0;
        for (j = 0; j < matrices_data.cols[MATRIX_A]; j++) {
            cum += matrices_data.matrices[MATRIX_A][idx][j] * matrices_data.matrices[MATRIX_B][j][i];
        }

        matrices_data.matrices[MATRIX_C][idx][i] = cum;
#if DEBUG
        printf("[%s:%d] matrices[%d][%d][%d]=%d\n", __func__, __LINE__, MATRIX_C, idx, i, cum);
#endif
    }

    return NULL;
}

int main(int argc, char **argv)
{
    /* argv[0] is argc = 1 */
    if (argc < 5) {
        handle_error_msg("[%s:%d] Usage: %s 4 4 4 4 (4 rows x 4 cols)\n", __func__, __LINE__, *argv);
        return EXIT_FAILURE;
    }

	return matrix_start(argv + 1);
}

/*
    1) idx = 0, matrix C: rows: 4 (from m A), cols: 4 (from m B)
    1 2 3   1 . . .     38 . . .
    . . .   5 . . .  -> .  . . .
    . . .   9 . . .     .  . . .
    . . .

    2)
    1 2 3   .  2 . .     38 44 . .
    . . .   .  6 . .  -> .  .  . .
    . . .   . 10 . .     .  .  . .
    . . .

    3)
    1 2 3   .  . 3  .     38 44 48 .
    . . .   .  . 7  .  -> .  . . .
    . . .   .  . 11 .     .  . . .
    . . .

    4)
    1 2 3   .  . .  4     38 44 50 56
    . . .   .  . .  8  -> .  .  .  .
    . . .   .  . .  12    .  .  .  .
    . . .

    5) idx = 1
    . . .   1  . .  .     38 44 48 56
    4 5 6   5  . .  .  -> 83  . .  .
    . . .   9  . .  .     .  .  .  .
    . . .

    6)
    . . .   . 2  .  .     38 44 48 56
    4 5 6   . 6  .  .  -> 83 98 .  .
    . . .   . 10 .  .     .  .  .  .
    . . .                 .  .  .  .

    7)
    . . .   . . 3  .     38 44 48  56
    4 5 6   . . 7  .  -> 83 98 113 .
    . . .   . . 11 .     .  .  .   .
    . . .                .  .  .   .

    8)
    . . .   . . .  4     38 44 48  56
    4 5 6   . . .  8  -> 83 98 113 128
    . . .   . . . 12     .  .  .   .
    . . .                .  .  .   .
*/

int matrix_start(char **argv)
{
	int ret = 0, i = 0, j = 0, l = 0;
    int thread_count = 0;
    pthread_t *threads = NULL;

    matrices_data.rows[i] = atoi(*argv);
    matrices_data.cols[i] = atoi(*(argv + 1));
    i++;
    matrices_data.rows[i] = atoi(*(argv + 2));
    matrices_data.cols[i] = atoi(*(argv + 3));
    i++;
    matrices_data.rows[i] = matrices_data.rows[0];
    matrices_data.cols[i] = matrices_data.cols[1];

    if (matrices_data.rows[0] <= 0 || matrices_data.rows[1] <= 0 ||
        matrices_data.cols[0] <= 0 || matrices_data.cols[1] <= 0) {
        handle_error_msg("[%s:%d] error: rows/cols cannot be less than / equal to zero. yours: %d %d %d %d\n", __func__, __LINE__, matrices_data.rows[0], matrices_data.cols[0], matrices_data.rows[1], matrices_data.cols[1]);
        ret = EXIT_FAILURE;
        goto err;
    } else if (matrices_data.cols[0] != matrices_data.rows[1]) {
        handle_error_msg("[%s:%d] error: cols of matrix 1 must be equal to the number of rows of the 2nd matrix: cols=%d rows=%d\n", __func__, __LINE__, matrices_data.cols[0], matrices_data.rows[1]);
        ret = EXIT_FAILURE;
        goto err;
    }

    srand(time(NULL));

    /* 4 rows x 3 cols:
        1  2  3
        4  5  6
        7  8  9
        10 11 12 */
    /* 3 rows x 4 cols:
       1  2  3  4
       5  6  7  8
       9 10 11 12 */

    for (i = 0; i < MATRIX_NUM; i++) {
        /* allocate memory for each int **: each ptr represents row */
        matrices_data.matrices[i] = calloc(matrices_data.rows[i], sizeof(int *));
        if (!matrices_data.matrices[i]) {
            handle_error("calloc", errno);
            ret = EXIT_FAILURE;
            goto err;
        }

        for (j = 0; j < matrices_data.rows[i]; j++) {
            /* allocate memory for each int * each ptr represents col */
            matrices_data.matrices[i][j] = calloc(matrices_data.cols[i], sizeof(int));
            if (!matrices_data.matrices[i][j]) {
                handle_error("calloc", errno);
                ret = EXIT_FAILURE;
                goto err;
            }

            if (i < MATRIX_NUM - 1) {
                for (l = 0; l < matrices_data.cols[i]; l++)
                    matrices_data.matrices[i][j][l] = rand() % MAX_NUM;
                }
        }
    }

    threads = calloc(matrices_data.rows[MATRIX_C], sizeof(pthread_t));
    if (!threads) {
        handle_error("calloc", errno);
        ret = EXIT_FAILURE;
        goto err;
    }

    for (i = 0; i < matrices_data.rows[MATRIX_C]; i++) {
        ret = pthread_create(&threads[i], NULL, multiply_func, (void *)(uintptr_t)i);
        if (ret != 0) {
            handle_error("pthread_create", errno);
            ret = EXIT_FAILURE;
            goto err; // don't join threads that has never existed
        }
        thread_count++;
    }

    for (i = 0; i < thread_count; i++) {
        ret = pthread_join(threads[i], NULL);
        if (ret != 0)
            handle_error("pthread_join", errno);
    }

    for (i = 0; i < MATRIX_NUM; i++) {
        printf("-----%c------\n", i == 0 ? 'A' : i == 1 ? 'B' : 'C');
        display_matrix(matrices_data.matrices[i], matrices_data.rows[i], matrices_data.cols[i]);
        printf("-------------\n");
    }

    thread_count = 0;

err:
    for (i = 0; i < thread_count; i++) {
        ret = pthread_join(threads[i], NULL);
        if (ret != 0)
            handle_error("pthread_join", errno);
    }

    free(threads);
    for (i = 0; i < MATRIX_NUM; i++) {
        if (matrices_data.matrices[i]) {
            for (j = 0; j < matrices_data.rows[i]; j++)
                free(matrices_data.matrices[i][j]); // free(NULL) is a safe OP
            free(matrices_data.matrices[i]);
        }
    }

    return ret;
}

void display_matrix(int **matrix, int rows, int cols)
{
    int i, j;
    printf("%d x %d\n", rows, cols);
    /* moving down along rows */
    for (i = 0; i < rows; i++) {
        /* moving across columns */
        for (j = 0; j < cols; j++)
            printf(" %d ", matrix[i][j]);
        putchar('\n');
    }
}
