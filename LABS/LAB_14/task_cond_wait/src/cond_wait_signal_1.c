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
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
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

        if (car >= TANK_CAPACITY) {
            printf("Customer is satisfied. No need to wait\n");
            (void)pthread_cond_signal(&cond);            
            (void)pthread_mutex_unlock(&mutex);
            break;
        }

        while (fuel >= GAS_STORAGE) {
            printf("Enough fuel at the gas station. Wait for signal from customer\n");
            /* Customer blocks and realeases mutex until reception of cond signal */
            /* pthread_cond_wait should always be placed inside a loop due to spurious wake-up calls */
            /* wake-up calls result in ret = 0 while no change to `fuel` has occurred */
            if ((ret = pthread_cond_wait(&cond, &mutex)) != 0) {
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
            (void)pthread_cond_signal(&cond);            
            (void)pthread_mutex_unlock(&mutex);
            break;
        }

        fuel = 35;
        printf("refilling done. Notify customer\n");

        (void)pthread_cond_signal(&cond);

        if ((ret = pthread_mutex_unlock(&mutex)) != 0) {
            handle_error("pthread_mutex_unlock", errno);
            break;
        }

        // sleep(1); // the impact is almost zero
        /* Without any sleep to delay the execution of both threads, we solely rely on cond variable */
        /* If refills signals, unlocks, and re-locks again before purchase, it enters pthread_cond_wait */
        /* If both threads end up waiting in pthread_cond_wait, a deadlock will occur */
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

        while (fuel == 0) {
            printf("No fuel at the gas station is empty. Wait for it to be refilled\n");
            /* Customer blocks and realeases mutex until reception of cond signal */
            if ((ret = pthread_cond_wait(&cond, &mutex)) != 0) {
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
            (void)pthread_cond_signal(&cond);
            (void)pthread_mutex_unlock(&mutex);
            break;
        }

        (void)pthread_cond_signal(&cond);

        if ((ret = pthread_mutex_unlock(&mutex)) != 0)
            handle_error("pthread_mutex_unlock", errno);

        // sleep(5);
        /* Without any sleep to delay the execution of both threads, we solely rely on cond variable */
        /* If purchase signals, unlocks, and re-locks again before refill, it enters pthread_cond_wait */
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
    if ((ret = pthread_cond_destroy(&cond)) != 0)
        handle_error("pthread_cond_destroy", errno);

    if ((ret = pthread_mutex_destroy(&mutex)) != 0)
        handle_error("pthread_mutex_destroy", errno);
#endif

    return EXIT_SUCCESS;
}
