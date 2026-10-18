#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#define CUSTOMER 0
#define GAS_STATION 1
#define THREAD_NUM 2
#define GAS_STORAGE 35
#define TANK_CAPACITY 75
#define BUF_SIZ 256
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s", __func__, __LINE__, (msg), err_str); \
                                } while(0)
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/* Separate conditions to break the deadlock loop */
static pthread_cond_t cond_station_empty = PTHREAD_COND_INITIALIZER; // For customer to wait
static pthread_cond_t cond_station_full  = PTHREAD_COND_INITIALIZER; // For refiller to wait
static int fuel = 35;
static int car = 0;

void *refill(void *args)
{
    int ret = 0;
    (void)args;

    while (1) {
        if ((ret = pthread_mutex_lock(&mutex)) != 0) {
            handle_error("pthread_mutex_lock", errno);
            return NULL;
        }


        /* Wait while there's enough fuel, car is not full and until cond state is changed */
        while (fuel >= GAS_STORAGE && car < TANK_CAPACITY) {
            printf("Enough fuel at the gas station. Wait for signal from customer\n");
            /* Customer blocks and realeases mutex until reception of cond signal */
            /* pthread_cond_wait should always be placed inside a loop due to spurious wake-up calls */
            /* wake-up calls result in ret = 0 while no change to `fuel` has occurred */
            if ((ret = pthread_cond_wait(&cond_station_full, &mutex)) != 0) {
                (void)pthread_mutex_unlock(&mutex);
                handle_error("pthread_cond_wait", errno);
                break;
            }
            /*  under Linux and POSIX-compliant systems, pthread_cond_wait can occasionally wake up and return 0
                even if no one called pthread_cond_signal.
                This is caused by low-level kernel optimizations and hardware interrupts
            */
        }

        if (car >= TANK_CAPACITY) {
            printf("Customer is satisfied. No need to wait\n");
            (void)pthread_cond_signal(&cond_station_empty);            
            (void)pthread_mutex_unlock(&mutex);
            break;
        }

        fuel = 35;
        printf("refilling done. Notify customer\n");

        (void)pthread_cond_signal(&cond_station_empty);

        if ((ret = pthread_mutex_unlock(&mutex)) != 0) {
            handle_error("pthread_mutex_unlock", errno);
            break;
        }
    }

    return NULL;
}

void *purchase(void *args)
{
    int ret = 0;

    (void)args;

    while (1) {
        if ((ret = pthread_mutex_lock(&mutex)) != 0) {
            handle_error("pthread_mutex_lock", errno);
            break;
        }

        /* Wait while there's no fuel until cond state is changed */
        while (fuel == 0) {
            printf("No fuel at the gas station is empty. Wait for it to be refilled\n");
            /* Customer blocks and realeases mutex until reception of cond signal */
            if ((ret = pthread_cond_wait(&cond_station_empty, &mutex)) != 0) {
                (void)pthread_mutex_unlock(&mutex);
                handle_error("pthread_cond_wait", errno);
                return NULL;
            }
        }

        car += fuel;
        fuel = 0;
        printf("Car tank is %d%% filled\n", car);

        if (car >= TANK_CAPACITY) {
            printf("Car tank is full. Leaving gas station\n");
            (void)pthread_cond_signal(&cond_station_full);
            (void)pthread_mutex_unlock(&mutex);
            break;
        }

        /* POSIX signals have no memory, so if no thread waits in pthred_cond_wait, the signal is lost */
        (void)pthread_cond_signal(&cond_station_full);
        /* Losing a signal because thread is blocked on pthread_mutex_lock is fine
           As long as there is no deadlock */
        /* We need condition variables to properly synchronize an interaction between threads
            Since both depend on each other states/variables (car and fuel) */

        if ((ret = pthread_mutex_unlock(&mutex)) != 0)
            handle_error("pthread_mutex_unlock", errno);
    }

    return NULL;
}

int main(void)
{
    int ret = 0, i = 0;
    pthread_t threads[THREAD_NUM];
    void *(*func)(void *) = NULL;

    for (i = 0; i < THREAD_NUM; i++) {
        // first we attempt to purchase in order to make sure there's no gas at the gas station
        func = (i == 0) ? purchase : refill;
        if ((ret = pthread_create(&threads[i], NULL, func, NULL)) != 0)
            handle_error("pthread_create", errno);
    }

    for (i = 0; i < THREAD_NUM; i++)
        if ((ret = pthread_join(threads[i], NULL)) != 0)
            handle_error("pthread_join", errno);

/* statically initialized mutexes and conditional variables need no deinit routine*/
#if 0
    if ((ret = pthread_cond_destroy(&cond_station_empty)) != 0)
        handle_error("pthread_cond_destroy", ret);

    if ((ret = pthread_cond_destroy(&cond_station_full)) != 0)
        handle_error("pthread_cond_destroy", ret);

    if ((ret = pthread_mutex_destroy(&mutex)) != 0)
        handle_error("pthread_mutex_destroy", ret);
#endif

    return EXIT_SUCCESS;
}
