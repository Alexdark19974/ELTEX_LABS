#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#define THREAD_NUM 10
#define BUF_SIZ 256
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)
#define handle_error_msg(msg) do { \
                            fprintf(stderr, "[%s:%d] error: %s\n", __func__, __LINE__, (msg)); \
                            exit(EXIT_FAILURE); \
                        } while(0)

static pthread_once_t once_control = PTHREAD_ONCE_INIT;
static pthread_key_t ctx_key;

typedef struct ThreadContext_s {
    int thread_num;
    int transaction_count;
} ThreadContext_t;

void flush_transactions(void)
{
    ThreadContext_t *ctx = NULL;

/* No need to add parameters to funcs as pthread_getspecific handles data access */
#if 0
    if (!(ctx = (ThreadContext_t *)pthread_getspecific(ctx_key))) {
        handle_error("pthread_getspecific", errno);
        return;
    }
#endif
    /* We operate on asssumption that ctx_key is valid and malloc has done its job in worker_func() */
    ctx = (ThreadContext_t *)pthread_getspecific(ctx_key);
    if (!ctx) // paranoia case: this should never happen under normal circumstances
        pthread_exit(NULL);
    ctx->transaction_count = 0;

    printf("[tid:%d] transactions flushed\n", ctx->thread_num);
}

void do_transaction(void)
{
    ThreadContext_t *ctx = NULL;
#if 0
     if (!(ctx = (ThreadContext_t *)pthread_getspecific(ctx_key))) {
        handle_error("pthread_getspecific", errno);
        return;
     }
#endif
    ctx = (ThreadContext_t *)pthread_getspecific(ctx_key);
    if (!ctx) // paranoia case: this should never happen under normal circumstances
        pthread_exit(NULL);
    ctx->transaction_count++;

    printf("[tid:%d] transaction_count = %d\n", ctx->thread_num, ctx->transaction_count);
}

void destr_func(void *args)
{
    ThreadContext_t *ctx = args;
    printf("[tid:%d] addr %p: freed data.\n", ctx->thread_num, (void *)ctx);
    /* free allocated resources in worker_func and passed to pthread_setspecific() */
    free(args);
}

/* Each thread possesses TSD (thread-specific data) area, index by TSD keys */
void make_key(void)
{
    int ret;

    printf("make_key called\n");

    /* create thread specific data, unique to each thread and not shared */
    /* key is used to acccess and index that data which is uniqure for every thread */
    /* destr_func as 2nd arg is used for a clean-up: freeing data if any */
    if ((ret = pthread_key_create(&ctx_key, destr_func)) != 0)
       handle_error("pthread_key_create", errno);
    /* 1 global variable key as an index for data */
    /* multiple unique and thread-specific data locations */
}

void *worker_func(void *args)
{
    ThreadContext_t *ctx = NULL;
    int ret;

    /* any subsequent calls with the same once_control shall not call make_key */
    /* make_key is invoked once, other threads ignore it or blocked untill make_key routine is done */
    if ((ret = pthread_once(&once_control, make_key)) != 0) {
        handle_error("pthread_once", errno);
        return NULL;
    }

    /* Allocate thread-specific memory accessible only to a single thread */
    if (!(ctx = malloc(sizeof(ThreadContext_t)))) {
        handle_error("malloc", errno);
        return NULL;                     
    }

    ctx->thread_num = (int)(uintptr_t)args;
    ctx->transaction_count = 0;

    /* Now bind key to ctx to access it in sub-routines within a thread */
    /* After the bind is done, no no need to pass ctx as a parameter to a routine */
    /* The operating system maintains a hidden internal table
        that maps threads to their private pointers using the key as a row selector
        thread 1 + key -> pointer to data A
        thread 2 + key -> pointer to data B */
    if ((ret = pthread_setspecific(ctx_key, ctx)) != 0) {
        handle_error("pthread_setspecific", errno);
        free(ctx);
        return NULL;

    }

    do_transaction();
    do_transaction();
    do_transaction();
    flush_transactions();

    return NULL; // pthread_exit(NULL) performs the same function in this case
    /* But if sub-functions within func() are present,
        and one of them calls pthread_exit(), the whole thread is terminated */
}

int main(void)
{
    int ret, i, j;
    pthread_t threads[THREAD_NUM];

    j = 1;

    for (i = 0; i < THREAD_NUM; i++, j++) {
        /* passing arg by value converted to uintptr_t and cast to void * */
        if ((ret = pthread_create(&threads[i], NULL, worker_func, (void *)(uintptr_t)j)) != 0)
            handle_error("pthread_create", errno);
    }

    for (i = 0; i < THREAD_NUM; i++)
        if ((ret = pthread_join(threads[i], NULL)) != 0)
            handle_error("pthread_join", errno);

    (void)pthread_key_delete(ctx_key);

    return EXIT_SUCCESS;
}
