#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#define STORE_NUM 3
#define MAX_DEMAND 5500
#define MIN_SUPPLY 500
#define THREAD_NUM 5
#define CUSTOMER_NUM THREAD_NUM
#define BUF_SIZ 256
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)

typedef struct customer_s {
    pthread_t thread_id;
    int cust_id;
    int demand;
} customer_t;

typedef struct store_s {
    pthread_t thread_id;
    int store_id;
    int goods;
} store_t;

static bool stores_open = true;
static store_t stores[STORE_NUM];
static pthread_mutex_t mutex[STORE_NUM] = { PTHREAD_MUTEX_INITIALIZER };
static pthread_mutex_t flag_mutex = PTHREAD_MUTEX_INITIALIZER;

void *purchase_start(void *args)
{
    customer_t *customer = NULL;
    int num = 0;

    if (!args)
        return NULL;

    customer = (customer_t *)args;

    while (customer->demand > 0) {
        num = rand() % STORE_NUM;

        (void)pthread_mutex_lock(&mutex[num]);

        printf("[cid:%d] shopping at store%d\n", customer->cust_id, stores[num].store_id);
    
        customer->demand -= stores[num].goods;
        stores[num].goods = 0;

        printf("[cid:%d] demand is %d\n", customer->cust_id, customer->demand);

        (void)pthread_mutex_unlock(&mutex[num]);

        sleep(2);
    }

    return NULL;
}

void *supply_start(void *args)
{
    int num = 0;

    (void)args;

    while (true) {
        num = rand() % STORE_NUM;

        (void)pthread_mutex_lock(&mutex[num]);

        stores[num].goods += 500;

        printf("[sid:%d] Resupplied: %d now\n", stores[num].store_id, stores[num].goods);

        /* putting stores_open read and write ops in mutex
           we force compilers to turn off optimizations that could
           put stores_opens's value in cache registers, reading it from RAM every time */
        (void)pthread_mutex_lock(&flag_mutex);
        /* stores_open cannot be optimized away while being a part of critical section */
        if (stores_open == false) {
            (void)pthread_mutex_unlock(&flag_mutex);
            (void)pthread_mutex_unlock(&mutex[num]);
            break;            
        }
        (void)pthread_mutex_unlock(&flag_mutex);

        (void)pthread_mutex_unlock(&mutex[num]);

        sleep(1);
    }

    return NULL;
}

int main(void)
{
    int ret, i;
    customer_t customers[CUSTOMER_NUM];

    /* Generate random seed for rand() every time the program starts */
    /* We should call it once, at the very beginning */
    srand(time(NULL));

    for (i = 0; i < CUSTOMER_NUM; i++) {
        customers[i].cust_id = i + 1;
        customers[i].demand = rand() % MAX_DEMAND;
        printf("[%s:%d] customer%d, demand=%d\n", __func__, __LINE__, customers[i].cust_id, customers[i].demand);
        if ((ret = pthread_create(&customers[i].thread_id, NULL, purchase_start, &customers[i])) != 0)
            handle_error("pthread_create", errno);
    }

    for (i = 0; i < STORE_NUM; i++) {
        stores[i].store_id = i + 1;
        stores[i].goods = rand() % MIN_SUPPLY;
        printf("[%s:%d] store%d, goods=%d\n", __func__, __LINE__, stores[i].store_id, stores[i].goods);
        if ((ret = pthread_create(&stores[i].thread_id, NULL, supply_start, NULL)) != 0)
            handle_error("pthread_create", errno);
    }

    for (i = 0; i < CUSTOMER_NUM; i++) {
        if ((ret = pthread_join(customers[i].thread_id, NULL)) != 0) {
            handle_error("pthread_join", errno);
        }
    }

    printf("All customers are satisfied.\n");

    /* Unsafe to use if worker threads are cancelled while mutex is locked: mutex leaked
       Or another thread deadlocking on re-acquiring the mutex */
#if 0
    for (i = 0; i < STORE_NUM; i++)
        (void)pthread_cancel(stores[i].thread_id);
#endif
    /* global mutex-protected read-only variable makes sure each thread can terminate leak-free */
    /* data race: multilple reads and at least one write at the same time.
       By adding a separate mutex for the flag, we eliminate the very possibility. */
    /* opaque functions such as pthread_mutex_lock force compilers to turn off aggressive optimizations
       i.e. no register caches */
    (void)pthread_mutex_lock(&flag_mutex);
    stores_open = false;
    (void)pthread_mutex_unlock(&flag_mutex);

    for (i = 0; i < STORE_NUM; i++) {
        if ((ret = pthread_join(stores[i].thread_id, NULL)) != 0)
            handle_error("pthread_join", errno);
    }

    /* There's a chance that we will deadlock on this printf because of pthread_cancel() */
    printf("All stores are closed.\n");

    return EXIT_SUCCESS;
}
