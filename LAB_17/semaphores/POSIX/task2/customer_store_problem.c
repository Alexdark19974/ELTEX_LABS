#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#define SEGSIZE 4096
#define CUSTOMER_MAX 3
#define STORE_MAX 2
#define TRUE 1
#define randomizer() ((rand()) % STORE_MAX)
#define error_check(errn, msg) \
	 	do {if (-1 == errn) {perror(msg); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);} } while(0)
#define error_check_ptr(msg) \
	 	do { perror(msg); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);} while(0)

/*
 *
 * Customer 1 = 5000 units.
 * Customer 2 = 5100 units.
 * Customer 3 = 4500 units.
 * Store 1 = 1000
 * Store 2 = 950
 * Supplier = + 500 units.
 * A customer takes all units from Store.
 *
 * If done right, no deadlock will occur.
 *
 * */

void stores_customers_init(void);
void customer(int, sem_t *, sem_t *, int *);
void supplier(sem_t *, sem_t *, int *);

int main(void)
{
	stores_customers_init();
	exit(EXIT_SUCCESS);
}

void stores_customers_init(void)
{
	int shmfd = shm_open("/customer_store_problem.c", O_CREAT | O_EXCL | O_RDWR, 0666);
	error_check(shmfd, "shm_open");

	int state = ftruncate(shmfd, SEGSIZE);
	error_check(shmfd, "ftruncate");

	int *data_ptr = mmap(NULL, SEGSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	error_check(*data_ptr, "mmap");

	data_ptr[0] = 1000;
	data_ptr[1] = 950;
	data_ptr[2] = 5000;
	data_ptr[3] = 5100;
	data_ptr[4] = 4500;
	data_ptr[5] = 500;

	sem_t *smphr_1 = sem_open("/semprog.c", O_CREAT | O_EXCL, 0666, 1);
	if (smphr_1 == SEM_FAILED) { error_check_ptr("sem_open"); }
	sem_t *smphr_2 = sem_open("shmreader.c", O_CREAT | O_EXCL, 0666, 1);
	if (smphr_2 == SEM_FAILED) { error_check_ptr("sem_open"); }

	int cust_pid[3] = {0};

	srand(0);

	cust_pid[0] = fork();
	error_check(cust_pid[0], "fork");

	if (0 == cust_pid[0])
	{
		customer(2, smphr_1, smphr_2, data_ptr);
	}

	cust_pid[1] = fork();
	error_check(cust_pid[1], "fork");

	if (0 == cust_pid[1])
	{
		customer(3, smphr_1, smphr_2, data_ptr);
	}

	cust_pid[2] = fork();
	error_check(cust_pid[2], "fork");

	if (0 == cust_pid[2])
	{
		customer(4, smphr_1, smphr_2, data_ptr);
	}

	int suppl_pid = fork();
	error_check(suppl_pid, "fork");

	if (0 == suppl_pid)
	{
		supplier(smphr_1, smphr_2, data_ptr);
	} 

	int wstatus = 0;
	for (int i = 0; i < CUSTOMER_MAX; i++)
	{
		wstatus = wait(&cust_pid[i]);
		error_check(wstatus, "wait");
	}

	wstatus = kill(suppl_pid, SIGTERM);
	error_check(wstatus, "kill");

	printf("All customers have been satisfied.\n");
	
	state = munmap(data_ptr, SEGSIZE);
	error_check(state, "munmap");

	state = shm_unlink("/customer_store_problem.c");
	error_check(state, "unlink");

	state = sem_close(smphr_1);
	error_check(state, "sem_close");

	state = sem_unlink("/semprog.c");
	error_check(state, "sem_unlink");

	state = sem_close(smphr_2);
	error_check(state, "sem_close");

	state = sem_unlink("shmreader.c");
	error_check(state, "sem_unlink");
}

void customer(int cstm_nmbr, sem_t *semphr_1, sem_t *semphr_2, int *data_ptr)
{
	int store_nmbr = randomizer();
	int c_pid = getpid();

	while (data_ptr[cstm_nmbr] > 0)
	{
		if (0 == store_nmbr)
		{
			sem_wait(semphr_1);
		}
		else
		{
			sem_wait(semphr_2);
		}

		printf ("Customer %d (PID: %d) with demand %d entered store %d.\n", cstm_nmbr - 1, c_pid, data_ptr[cstm_nmbr], store_nmbr);
		sleep(1);

		if (0 != data_ptr[store_nmbr])
		{
			data_ptr[cstm_nmbr]-= data_ptr[store_nmbr];
			data_ptr[store_nmbr] = 0;
			printf ("Customer %d bought goods at store %d. Demand is %d now.\n", cstm_nmbr - 1, store_nmbr, data_ptr[cstm_nmbr]);
		}
		else
		{
			printf("Store is empty...\n");
		}

		if (0 == store_nmbr)
		{
			sem_post(semphr_1);
		}
		else
		{
			sem_post(semphr_2);
		}

		printf("Left store %d.\n", store_nmbr);
		sleep(2);
	}

	printf("Customer %d is satisfied.\n", cstm_nmbr - 1);

	int state = munmap(data_ptr, SEGSIZE);
	error_check(state, "munmap");

	if (0 == store_nmbr)
	{
		state = sem_close(semphr_1);
		error_check(state, "sem_close");
	}
	else
	{
		sem_close(semphr_2);
		error_check(state, "sem_close");
	}

	exit(EXIT_SUCCESS);
}

void supplier(sem_t *semphr_1, sem_t *semphr_2, int *data_ptr)
{
	int store_nmbr = randomizer();

	while (TRUE)
	{
		if (0 == store_nmbr)
		{
			sem_wait(semphr_1);
		}
		else
		{
			sem_wait(semphr_2);
		}

		printf("Supplier entered store %d\n", store_nmbr);

		sleep(1);

		data_ptr[store_nmbr]+= 500;

		printf("Refilled.\n");

		if (0 == store_nmbr)
		{
			sem_post(semphr_1);
		}
		else
		{
			sem_post(semphr_2);
		}

		sleep(5);
	}
}