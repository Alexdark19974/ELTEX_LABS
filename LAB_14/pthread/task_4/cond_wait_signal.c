#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#define CUSTOMER 0
#define GAS_STATION 1
#define THREAD_MAX 2
#define GAS_STORAGE 35
#define TANK_CAPACITY 75
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
        do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

typedef struct Thread
{
    pthread_t thread_id;
} Thread;

pthread_mutex_t mutex;
pthread_cond_t cond;
int fuel = 0;
int car = 0;

void gas_station_customer(void);

void *refill(void *const args)
{
    while (car < TANK_CAPACITY)
    {
        int state = 0;
        state = pthread_mutex_lock(&mutex);

        if (0 != state)
        {
            handle_error_en(state, "pthread_mutex_lock");
        }

        while (fuel < GAS_STORAGE)
        {
            fuel+=15;
            printf(" gas staton refilled %d.\n", fuel);
            sleep(1);
        }

        printf(" refilling done.\n");

        state = pthread_cond_signal(&cond);
        sleep(1);

        if (0 != state)
        {
            handle_error_en(state, "pthread_cond_signal");
        }

        state = pthread_mutex_unlock(&mutex);

        if (0 != state)
        {
            handle_error_en(state, "pthread_mutex_unlock");
        }

        sleep(1);
    }

    pthread_exit(NULL);
}
void *purchase(void *const args)
{
    int state = 0;
    state = pthread_mutex_lock(&mutex);

    if (0 != state)
    {
        handle_error_en(state, "pthread_mutex_lock");
    }

    while (car <= TANK_CAPACITY)
    {
        if (fuel > 0)
        {
            car+=fuel;
            fuel = 0;
            printf(" tank filled %d.\n", car);
            printf(" %d left at gas station.\n", fuel);
        }
        else
        {
            printf(" gas station is empty. Need to refill.\n");
            sleep(1);
            state  = pthread_cond_wait(&cond, &mutex);

            if (0 != state)
            {
                handle_error_en(state, "pthread_cond_wait");
            }
        }

        sleep(1);
    }

    state = pthread_mutex_unlock(&mutex);

    if (0 != state)
    {
	handle_error_en(state, "pthread_mutex_unlock");
    }

    pthread_exit(NULL);
}

int main (void)
{
    gas_station_customer();

    exit(EXIT_SUCCESS);
}

void gas_station_customer(void)
{
    int state = 0;
    struct Thread *thread_ptr = calloc(THREAD_MAX, sizeof(struct Thread));

    if (thread_ptr == NULL)
    {
        handle_error("calloc");
    }

    state = pthread_mutex_init(&mutex, NULL);

    if (0 != state)
    {
        handle_error_en(state, "pthread_mutex_init");
    }

    state = pthread_cond_init(&cond, NULL);

    if (0 != state)
    {
        handle_error_en(state, "pthread_mutex_cond");
    }

    for (int i = 0 ; i < THREAD_MAX; i++)
    {
        if (0 == i)
        {
            state = pthread_create(&thread_ptr[i].thread_id, NULL, &purchase, NULL);

            if (0 != state)
            {
                handle_error_en(state, "pthread_create");
            }
        }
        else
        {
            state = pthread_create(&thread_ptr[i].thread_id, NULL, &refill, NULL);

            if (0 != state)
            {
                handle_error_en(state, "pthread_create");
            }
        }
    }

    for (int i = 0; i < THREAD_MAX; i++)
    {
        state = pthread_join(thread_ptr[i].thread_id, NULL);

        if (0 != state)
        {
            handle_error_en(state, "pthread_join");
        }
    }

    state = pthread_cond_destroy(&cond);

    if (0 != state)
    {
        handle_error_en(state, "pthread_cond_destroy");
    }

    state = pthread_mutex_destroy(&mutex);

    if (0 != state)
    {
        handle_error_en(state, "pthread_mutex_destroy");
    }
}

