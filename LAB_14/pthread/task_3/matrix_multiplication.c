#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#define MATRIX_A_B 3
#define MATRIX_C 4
#define R_MAX 5
#define R_MIN 1
#define ERR -2
#define MAX_THREAD_NUM 2
#define randomizer() (rand() % (R_MAX + R_MIN))

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

typedef struct thread
{
	pthread_t thread_id;
	int **matrix_A;
    int **matrix_B;
    int **matrix_C;
	int thread_num;
	int rows;
	int cols;
	int rows_lim;
	int cols_lim;

} Thread;

int matrix_data_init(int **, int, int, int);
void t_matrix_data_init(Thread **, int **, int **, int **, int, int, int, int);
void display_matrix(int **, int **, int **, int, int, int, int);
void matrix_start(char **);
void argv_check(char **);

void *multiplication(void * const args)
{
	Thread *matrix_ptr = (Thread *) args;
	int counter = 0;
	int elements_arr[matrix_ptr->rows * matrix_ptr->cols];
	int lim = matrix_ptr->rows;
	int ops_max, cols_A, rows_A, cols_B, rows_B, cols_C, rows_C;
	ops_max = cols_A = rows_A = cols_B = rows_B = cols_C = rows_C = 0;

	if (matrix_ptr->thread_num == 0)
	{
		ops_max = (matrix_ptr->rows_lim * matrix_ptr->cols_lim) / 2;
	}

	while (counter != ops_max && matrix_ptr->thread_num == 0)
	{

		for (int n = 0; rows_A < matrix_ptr->rows && cols_B < matrix_ptr->cols; n++)
		{
			elements_arr[n] = matrix_ptr->matrix_A[cols_A][rows_A] * matrix_ptr->matrix_B[cols_B][rows_B];
			rows_A++;
			cols_B++;
		}

		for (int n = 1, k = 1; n < lim; k++, n++)
		{
			elements_arr[0] = elements_arr[0] + elements_arr[n];
			elements_arr[k] = 0;
		}

		matrix_ptr->matrix_C[cols_C][rows_C] = elements_arr[0];
		rows_C++;

		if (rows_C == matrix_ptr->rows_lim)
        {
	        rows_C = 0;
            cols_C++;
        }

		counter++;
        rows_A = 0;
        cols_B = 0;
        rows_B++;

		if (rows_B == matrix_ptr->rows_lim)
        {
            rows_B = 0;
			cols_A++;
        }
	}

	if (matrix_ptr->thread_num == 1)
    {
        if (((matrix_ptr->cols_lim * matrix_ptr->rows_lim) % 2) != 0)
        {
            ops_max = ((matrix_ptr->rows_lim * matrix_ptr->cols_lim) / 2) + 1;
        }
        else
        {
            ops_max = (matrix_ptr->rows_lim * matrix_ptr->cols_lim) / 2;
        }

        cols_A = matrix_ptr->cols_lim / 2;
		cols_C = matrix_ptr->cols_lim / 2;

		if ((matrix_ptr->rows_lim % 2) != 0 && (matrix_ptr->cols_lim % 2) != 0)
		{
			rows_B = matrix_ptr->rows_lim / 2;
			rows_C = matrix_ptr->rows_lim / 2;
		}
	}

	while (counter != ops_max && matrix_ptr->thread_num == 1)
    {

        for (int n = 0; cols_B < matrix_ptr->cols; n++)
        {
            elements_arr[n] = matrix_ptr->matrix_A[cols_A][rows_A] * matrix_ptr->matrix_B[cols_B][rows_B];
            rows_A++;
            cols_B++;
        }

        for (int n = 1, k = 1; n < matrix_ptr->cols; k++, n++)
        {
            elements_arr[0] = elements_arr[0] + elements_arr[n];
            elements_arr[k] = 0;
        }

        matrix_ptr->matrix_C[cols_C][rows_C] = elements_arr[0];
        rows_C++;

        if (rows_C == matrix_ptr->rows_lim)
        {
            rows_C = 0;
            cols_C++;
        }

        counter++;
        rows_A = 0;
        cols_B = 0;
        rows_B++;

        if (rows_B == matrix_ptr->rows_lim)
        {
            rows_B = 0;
            cols_A++;

			if (cols_A == matrix_ptr->cols_lim)
			{
				break;
			}
        }
    }

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        fprintf(stderr, " Usage: %s %d %d %d %d.\n", *argv, 4, 4, 4, 4);
    }

	matrix_start(argv);

	exit(EXIT_SUCCESS);
}

void matrix_start(char **argv)
{
    argv_check(argv);

	int state = 0;
	int cols_A = atoi(*(argv + 1));
	int rows_A = atoi(*(argv + 2));
	int cols_B = atoi(*(argv + 3));
	int rows_B = atoi(*(argv + 4));

    srand(time(0));

    if (cols_A <= 0 || rows_A <= 0 || cols_B <= 0 || rows_B <= 0)
    {
        fprintf(stderr, " error in file %s, line %d: rows/cols cannot be less than or equal to zero.\n", __FILE__, __LINE__);
        return;
    }

	int *A_matrix[cols_A];
    int *B_matrix[cols_B];
    int *C_matrix[cols_A];

	if ((state = matrix_data_init(A_matrix, cols_A, rows_A, MATRIX_A_B)) == ERR)
	{
		fprintf(stderr, " error: memory allocation failed in file %s, line %d.\n", __FILE__, __LINE__);
		return;
	}

	if ((state = matrix_data_init(B_matrix, cols_B, rows_B, MATRIX_A_B)) == ERR)
	{
        for (int i = 0; i < cols_A; i++)
        {
            free(A_matrix[i]);
            A_matrix[i] = NULL;
        }

		fprintf(stderr, " error: memory allocation failed in file %s, line %d.\n", __FILE__, __LINE__);
        return;
	}

    if ((state = matrix_data_init(C_matrix, cols_A, rows_B, MATRIX_C)) == ERR)
    {
        for (int i = 0; i < cols_A; i++)
        {
            free(A_matrix[i]);
            A_matrix[i] = NULL;

            if ((i + 1) == cols_A)
            {
                for (int j = 0; j < cols_B; j++)
                {
                    free(B_matrix[j]);
                    B_matrix[j] = NULL;
                }
            }
        }

        fprintf(stderr, " error: memory allocation failed in file %s, line %d.\n", __FILE__, __LINE__);
        return;
    }

	Thread *t_matrix = calloc(MAX_THREAD_NUM, sizeof(Thread));

    if (NULL == t_matrix)
    {
        for (int i = 0; i < cols_A; i++)
        {
            free(A_matrix[i]);
            A_matrix[i] = NULL;
            free(C_matrix[i]);
            C_matrix[i] = NULL;

            if ((i + 1) == cols_A)
            {
                for (int j = 0; j < cols_B; j++)
                {
                    free(B_matrix[j]);
                    B_matrix[j] = NULL;
                }
            }
        }

        handle_error("calloc");
    }

	t_matrix_data_init(&t_matrix, A_matrix, B_matrix, C_matrix, rows_A, cols_A, rows_B, cols_B);

	for (int i = 0; i < MAX_THREAD_NUM; i++)
	{
		state = pthread_create(&t_matrix[i].thread_id, NULL, &multiplication, &t_matrix[i]);

        if (0 != state)
        {
            for (int i = 0; i < cols_A; i++)
            {
                free(A_matrix[i]);
                A_matrix[i] = NULL;
                free(C_matrix[i]);
                C_matrix[i] = NULL;

                if ((i + 1) == cols_A)
                {
                    for (int j = 0; j < cols_B; j++)
                    {
                        free(B_matrix[j]);
                        B_matrix[j] = NULL;
                    }
                }
            }

            free(t_matrix);
            t_matrix = NULL;
            handle_error_en(state, "pthread_create");
        }
	}

	for (int i = 0; i < MAX_THREAD_NUM; i++)
	{
		state = pthread_join(t_matrix[i].thread_id, NULL);

        if (0 != state)
        {
            for (int i = 0; i < cols_A; i++)
            {
                free(A_matrix[i]);
                A_matrix[i] = NULL;
                free(C_matrix[i]);
                C_matrix[i] = NULL;

                if ((i + 1) == cols_A)
                {
                    for (int j = 0; j < cols_B; j++)
                    {
                        free(B_matrix[j]);
                        B_matrix[j] = NULL;
                    }
                }
            }

            free(t_matrix);
            t_matrix = NULL;
            handle_error_en(state, "pthread_join");
        }
	}

	display_matrix(A_matrix, B_matrix, C_matrix, cols_A, rows_A, cols_B, rows_B);

	for (int i = 0; i < cols_A; i++)
	{
		free(A_matrix[i]);
        A_matrix[i] = NULL;
		free(C_matrix[i]);
        C_matrix[i] = NULL;
		if ((i + 1) == cols_A)
		{
			for (int j = 0; j < cols_B; j++)
			{
				free(B_matrix[j]);
                B_matrix[j] = NULL;
			}
		}
	}

    free(t_matrix);
    t_matrix = NULL;
}

int matrix_data_init(int **matrix, int cols, int rows, int opt)
{
	if (opt == MATRIX_A_B)
	{
		for (int i = 0; i < cols; i++)
    	{
        	matrix[i] = calloc(rows, sizeof(int));

			if (matrix[i] == NULL)
			{
				return ERR;
			}

        	for (int j = 0; j < rows; j++)
        	{
           		matrix[i][j] = randomizer();
        	}
    	}

    }
	else
	{
		for (int i = 0; i < cols; i++)
    	{
        	matrix[i] = calloc(rows, sizeof(int));

            if (matrix[i] == NULL)
            {
                return ERR;
            }
    	}
    }

	return 0;
}

void argv_check(char **argv)
{
    int rows_A = atoi(*(argv + 2));
    int cols_B = atoi(*(argv + 3));

	if (*(argv + 1) == NULL)
    {
        fprintf(stderr, " error: missing the size of columns in Matrix A.\n");
        exit(EXIT_FAILURE);
    }

    if (*(argv + 2) == NULL)
    {
        fprintf(stderr, " error: missing the size of rows in Matrix A.\n");
        exit(EXIT_FAILURE);
    }

    if (*(argv + 3) == NULL)
    {
        fprintf(stderr, " error: missing the size of columns in Matrix B.\n");
        exit(EXIT_FAILURE);
    }

    if (*(argv + 4) == NULL)
    {
        fprintf(stderr, " error: missing the size of rows in Matrix B.\n");
        exit(EXIT_FAILURE);
    }

    if (rows_A != cols_B)
    {
        fprintf(stderr, " error: the numer of rows in matrix A must be equal to the number of column");
        exit(EXIT_FAILURE);
    }

}

void t_matrix_data_init(Thread **t_matrix, int **A_matrix, int **B_matrix, int **C_matrix, int rows_A, int cols_A, int rows_B, int cols_B)
{
	for (int i = 0; i < MAX_THREAD_NUM; i++)
    {
        if (i == 0)
        {
            t_matrix[0][i].thread_num = i;
            t_matrix[0][i].rows = rows_A;
            t_matrix[0][i].cols = cols_A;
        }
        else
        {
            t_matrix[0][i].thread_num = i;
            t_matrix[0][i].rows = rows_B;
            t_matrix[0][i].cols = cols_B;
        }

        t_matrix[0][i].cols_lim = cols_A;
        t_matrix[0][i].rows_lim = rows_B;
        t_matrix[0][i].matrix_A = A_matrix;
        t_matrix[0][i].matrix_B = B_matrix;
        t_matrix[0][i].matrix_C = C_matrix;
    }
}

void display_matrix(int **A_matrix, int **B_matrix, int **C_matrix, int cols_A, int rows_A, int cols_B, int rows_B)
{
	puts("before matrixes multiplied:");

    puts("---A---");

    for (int i = 0; i < cols_A; i++)
    {
        for (int j = 0; j < rows_A; j++)
        {
            printf(" %d ", A_matrix[i][j]);
        }
        puts("");
    }

    puts("---B---");

    for (int i = 0; i < cols_B; i++)
    {
        for (int j = 0; j < rows_B; j++)
        {
            printf(" %d ", B_matrix[i][j]);
        }
        puts("");
    }

    puts("after matrixes multiplied:");

    for (int i = 0; i < cols_A; i++)
    {
        for (int j = 0; j < rows_B; j++)
        {
            printf(" %d ", C_matrix[i][j]);
        }
        puts("");
    }
}
