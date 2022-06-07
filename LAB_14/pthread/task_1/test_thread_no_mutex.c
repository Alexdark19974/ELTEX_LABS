#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#define THREAD_SIZE 1000
#define INCREMENT_MAX 100000

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

typedef struct thread
{
	pthread_t thread_id;

} Thread;

void *thread_increment(void *args)
{
	long *tmp_ptr = (long *) args;
	long tmp = *tmp_ptr;

	for (int i = 0; i < INCREMENT_MAX; i++)
	{
		tmp++;
		*tmp_ptr = tmp;
	}

	printf("a = %ld\n", *tmp_ptr);

	return NULL;
}

int main(int argc, char *argv[])
{
	long a = 0;
	int s = 0;
	Thread *thread_ptr;

	thread_ptr = calloc(THREAD_SIZE, sizeof(Thread));

	if (thread_ptr == NULL)
	{
		handle_error("calloc");
	}

	for (int i = 0; i < THREAD_SIZE; i++)
	{
		thread_ptr[i].thread_id = i + 1;

		s = pthread_create(&thread_ptr[i].thread_id, NULL, &thread_increment, &a);

		if (s != 0)
		{
			handle_error_en(s, "pthread_create");
		}
	}

	for (int i = 0; i < THREAD_SIZE; i++)
	{
		s = pthread_join(thread_ptr[i].thread_id, NULL);

		if (s != 0)
        {
            handle_error_en(s, "pthread_create");
        }
	}

	printf(" the result of 1000 threads doing 100000 increments is %ld\n", a);

	free(thread_ptr);
	exit(EXIT_SUCCESS);
}
