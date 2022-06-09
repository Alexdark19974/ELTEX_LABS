#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#define MAX_THREAD 10
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
        do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

pthread_mutex_t mutex;
pthread_cond_t cond;

typedef struct Loop
{
    pthread_t thread_id;
    int thread_num;
} Loop;

long a = 0;

void pthread_wait_for_signal(void);

void *increment(void *const args)
{
    struct Loop *thread_ptr = args;

    for (int i = 0; i < 100; i++)
    {
        int s = 0;

        s = pthread_mutex_lock(&mutex);

        if (0 != s)
        {
            handle_error_en(s, "pthread_mutex_lock");
        }

        a++;

        printf(" incremented value %ld.\n", a);

        s = pthread_cond_signal(&cond);

        if (0 != s)
        {
            handle_error_en(s, "pthread_cond_signal");
        }

        s = pthread_mutex_unlock(&mutex);

        if (0 != s)
        {
            handle_error_en(s, "pthread_mutex_unlock");
        }

        if (thread_ptr->thread_num == MAX_THREAD)
        {
            int s = 0;

            s = pthread_mutex_lock(&mutex);

            printf(" thread %d blocked until certain condition is met.\n", thread_ptr->thread_num);

            if (0 != s)
            {
                handle_error_en(s, "pthread_cond_lock");
            }

            while (a < 900)
            {
                printf(" waiting...\n");
                sleep(1);
                s = pthread_cond_wait(&cond, &mutex);

                if (0 != s)
                {
                    handle_error_en(s, "pthread_cond_wait");
                }
            }

            printf(" condition is met.\n");

            for (int j = 0; j < 1000000; j++)
            {
                a++;
                printf(" incremented value %ld.\n", a);
            }

            s = pthread_mutex_unlock(&mutex);

            if (0 != s)
            {
                handle_error_en(s, "pthread_mutex_unlock");
            }

            break;
        }
    }

    pthread_exit(NULL);
}

int main(void)
{
    pthread_wait_for_signal();

    exit(EXIT_SUCCESS);
}

void pthread_wait_for_signal(void)
{
    int s = 0;
    struct Loop *loop_ptr = calloc(MAX_THREAD, sizeof(Loop));

    if (loop_ptr == NULL)
    {
        handle_error("calloc");
    }

    s = pthread_mutex_init(&mutex, NULL);

    if (0 != s)
    {
        handle_error_en(s, "pthread_mutex_init");
    }

    s = pthread_cond_init(&cond, NULL);

    for (int i = 0; i < MAX_THREAD; i++)
    {
        loop_ptr[i].thread_num = i + 1;
    }

    for (int i = 0; i < MAX_THREAD; i++)
    {
        s = pthread_create(&loop_ptr[i].thread_id, NULL, &increment, &loop_ptr[i]);
    }

    for (int i = 0; i < MAX_THREAD; i++)
    {
        s = pthread_join(loop_ptr[i].thread_id, NULL);
    }

    s = pthread_cond_destroy(&cond);

    if (0 != s)
    {
        handle_error_en(s, "pthread_cond_destroy");
    }

    s = pthread_mutex_destroy(&mutex);

    if (0 != s)
    {
        handle_error_en(s, "pthread_mutex_destroy");
    }

    free(loop_ptr);
    loop_ptr = NULL;
}
