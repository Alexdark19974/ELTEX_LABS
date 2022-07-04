#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#define TOP 100
#define SEGSIZE 4096
#define error_check(errn, msg) \
	 	do {if (-1 == errn) {perror(msg); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);} } while(0)
#define error_check_ptr(msg) \
	 	do { perror(msg); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);} while(0)

void increment(void);
void child(int *, sem_t *);

int main(void)
{
	increment();
	exit(EXIT_SUCCESS);
}

void increment(void)
{
	int shmfd = shm_open("/semprog.c", O_CREAT | O_EXCL | O_RDWR, 0666);
	error_check(shmfd, "shm_open");

	int state = ftruncate(shmfd, SEGSIZE);
	error_check(state, "ftruncate");

	int *incr_ptr = mmap(NULL, SEGSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	error_check(*incr_ptr, "mmap");

	*incr_ptr = 0;

	sem_t *smphr = sem_open("/customer_store_problem.c", O_CREAT | O_EXCL, 0666, 1);
	if (smphr == SEM_FAILED) { error_check_ptr("sem_open"); }

	int c_pid[2] = {0};

	c_pid[0] = fork();
	error_check(c_pid[0], "fork");

	if (0 == c_pid[0])
	{
		child(incr_ptr, smphr);
	}

	c_pid[1] = fork();
	error_check(c_pid[1], "fork");

	if (0 == c_pid[1])
	{
		child(incr_ptr, smphr);
	}

	/* 
		Wait for the child-processes to do their work
	*/

	int wstatus = wait(&c_pid[0]);
	error_check(wstatus, "wait");

	wstatus = wait(&c_pid[1]);
	error_check(wstatus, "wait");

	/* 
		Deallocate all the resources 
	*/

	state = munmap(incr_ptr, SEGSIZE);
	error_check(state, "munmap");

	state = shm_unlink("/semprog.c");
	error_check(state, "munmap");

	state = sem_close(smphr);
	error_check(state, "sem_close");

	state = sem_unlink("/customer_store_problem.c");
	error_check(state, "sem_unlink");
} 

void child(int *incr_ptr, sem_t *semphr)
{
	int c_pid = getpid();
	int p_pid = getppid();

	while (*incr_ptr < TOP)
	{
		sem_wait(semphr);

		printf(" Locked.\n");

		if (*incr_ptr >= TOP)
		{
			sem_post(semphr);

			/* 
				Release the resource and make it available
			*/

			/*
				Break out of the while loop if the incr_ptr has the same value as TOP
			*/
			
			break;

		}

		int counter = 0;

		while (counter < (TOP / 2))
		{
			*incr_ptr+=10;
			counter+=10;
			printf(" Child process %d incremented val by 10. incr_ptr = %d (PPID: %d)\n", c_pid, *incr_ptr, p_pid);
			sleep(1);
		}

		sem_post(semphr);

		printf(" Unlocked.\n");
		sleep(2);
	}

	printf(" Child process %d finished. (PPID: %d)\n", c_pid, p_pid);

	/* 
		Release the resources even though they are not going to be used now.
	*/

	int state = sem_close(semphr);
	error_check(state, "sem_close");

	state = munmap(incr_ptr, SEGSIZE);
	error_check(state, "munmap");

	exit(EXIT_SUCCESS);

	/* 
		Notify the parent-process that the child is done.
	*/
}