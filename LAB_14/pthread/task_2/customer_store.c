#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MIN 950
#define MAX 1050
#define STORES_MAX 5
#define STORES_MIN 1
#define DEMAND_MIN 4500
#define DEMAND_MAX 5500
#define CUSTOMERS_MAX 3

#define randomizer_1() (rand() % (MAX - MIN + 1) + MIN)
#define randomizer_2() (rand() % (DEMAND_MAX - DEMAND_MIN + 1) + DEMAND_MIN)
#define randomizer_3() (rand() % CUSTOMERS_MAX)
#define randomizer_4() (rand() % STORES_MAX)

#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
        do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

typedef struct Customer
{
    pthread_t thread_id;
    int number;
    int demand;
} Customer;

typedef struct Store
{
    int store_id;
    int goods;
} Store;

void init_data_customer_store(struct Customer**);
void customer_store_interaction(void);
void mutex_init(void);

pthread_mutex_t mutex[STORES_MAX];

struct Store stores[STORES_MAX] = {0};

void *purchase_start(void *const args)
{
    struct Customer *customer_ptr = args;

    while (customer_ptr->demand > 0)
    {
        int rand_res = randomizer_3();
        int s = 0;

        printf(" ------------------------\n");

        printf(" I am a customer %d. My demand is %d.\n", customer_ptr->number, customer_ptr->demand);

        s = pthread_mutex_lock(&mutex[rand_res]);

        if (0 != s)
        {
            fprintf(stderr, "error: failed to lock at %s, line %d", __FILE__, __LINE__);
            pthread_exit(NULL);
        }

        printf(" Now I am going to purchase at store %d.\n", stores[rand_res].store_id);
        customer_ptr->demand = customer_ptr->demand - stores[rand_res].goods;
        stores[rand_res].goods = 0;

        printf(" Now my demand is %d.\n", customer_ptr->demand);

        printf(" Store %d has %d now.\n", stores[rand_res].store_id, stores[rand_res].goods);

        printf(" ------------------------\n");

        s = pthread_mutex_unlock(&mutex[rand_res]);

        if (0 != s)
        {
            fprintf(stderr, "error: failed to unlock at %s, line %d", __FILE__, __LINE__);
            pthread_exit(NULL);
        }

        sleep(2);
    }

    pthread_exit(NULL);
}

void *supply_start(void *args)
{
    while (1)
    {
        int rand_res = randomizer_4();
        int s = 0;

        printf(" I am a supplier. I am going to refill store %d.\n", stores[rand_res].store_id);
        printf(" Its current supply is %d.\n", stores[rand_res].goods);

        s = pthread_mutex_lock(&mutex[rand_res]);

        if (0 != s)
        {
            fprintf(stderr, "error: failed to lock at %s, line %d", __FILE__, __LINE__);
            pthread_exit(NULL);
        }

        stores[rand_res].goods+= 500;

        printf(" I have refilled it and now it contains %d.\n", stores[rand_res].goods);
        printf(" Now I am leaving...\n");
        s = pthread_mutex_unlock(&mutex[rand_res]);

        if (0 != s)
        {
            fprintf(stderr, "error: failed to unlock at %s, line %d", __FILE__, __LINE__);
            pthread_exit(NULL);
        }

        sleep(1);
    }
}

int main(void)
{
    customer_store_interaction();

    exit(EXIT_SUCCESS);
}

void customer_store_interaction(void)
{
    struct Customer *t_customer = calloc(CUSTOMERS_MAX, sizeof(Customer));

    pthread_t supplier;

    int s = 0;

    if (t_customer == NULL)
    {
         handle_error("calloc");
    }

    srand(MIN);

    mutex_init();

    init_data_customer_store(&t_customer);

    for (int i = 0; i < CUSTOMERS_MAX; i++)
    {
        s = pthread_create(&t_customer[i].thread_id, NULL, &purchase_start, &t_customer[i]);

        if (0 != s)
        {
            free(t_customer);
            handle_error_en(s, "pthread_create");
        }
    }

    s = pthread_create(&supplier, NULL, &supply_start, NULL);

    if (0 != s)
    {
        pthread_cancel(t_customer[0].thread_id);
        pthread_cancel(t_customer[1].thread_id);
        pthread_cancel(t_customer[2].thread_id);
        free(t_customer);
        handle_error_en(s, "pthread_create");
    }


    for (int i = 0; i < CUSTOMERS_MAX; i++)
    {
        s = pthread_join(t_customer[i].thread_id, NULL);

        if (0 != s)
        {
            free(t_customer);
            handle_error_en(s, "pthread_join");
        }
    }

    printf(" All customers are satisfied.\n");

    s = pthread_cancel(supplier);

    if (0 != s)
    {
        free(t_customer);
        handle_error_en(s, "pthread_cancel");
    }

    s = pthread_join(supplier, NULL);

    if (0 != s)
    {
        free(t_customer);
        handle_error_en(s, "pthread_join");
    }

    for (int i = 0; i < CUSTOMERS_MAX; i++)
    {
       s = pthread_mutex_destroy(&mutex[i]);

        if (0 != s)
        {
            handle_error_en(s, "pthread_mutex_destroy");
        }
    }

    free(t_customer);
}

void init_data_customer_store(struct Customer **t_customer)
{
    for (int i = 0; i < STORES_MAX; i++)
    {
        stores[i].store_id = i + 1;
        stores[i].goods = randomizer_1();

        if (i < 3)
        {
            t_customer[0][i].number = i + 1;
            t_customer[0][i].demand = randomizer_2();
        }
    }

}

void mutex_init()
{
   for (int i = 0; i < STORES_MAX; i++)
   {
        int s = 0;
        s = pthread_mutex_init(&mutex[i], NULL);

        if (0 != s)
        {
           handle_error_en(s, "pthread_mutex_init");
        }
   }
}
