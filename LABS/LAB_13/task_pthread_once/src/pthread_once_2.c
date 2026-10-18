#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#define THREAD_NUM 4
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
static pthread_key_t key;

void destr_func(void *args)
{
    printf("addr %p: freed data.\n", args);
    free(args);
}

/* Each thread possesses TSD (thread-specific data) area, index by TSD keys */
void make_key(void)
{
    int ret;

    printf("make_key called\n");

    /* create thread specific data, unique to each thread and not shared */
    /* key is used to acccess and index that data */
    /* destr_func as 2nd arg is used for a clean-up: freeing data if any */
    if ((ret = pthread_key_create(&key, destr_func)) != 0)
       handle_error("pthread_key_create", errno);
}

void *func(void *args)
{
    int ret, i;
    void *data = NULL, *buf = NULL;

#if 0
    /* if we ever pass 0 as a valid integer, this line of code will mistakenly consider it invalid */
    if (!args) {
        handle_error_msg("error: no thread id supplied");
        return NULL;        
    }
#endif

    i = (int)(uintptr_t)args;

    /* any subsequent calls with the same once_control shall not call the init routne */
    /* make_key is invoked once */
    if ((ret = pthread_once(&once_control, make_key)) != 0) {
        handle_error("pthread_once", errno);
        return NULL;
    }

    /* See if there's any private thread-specific data associated with key */
    if (!(data = pthread_getspecific(key))) {

        if (!(buf = malloc(BUF_SIZ))) {
            handle_error("malloc", errno);
            return NULL;
        }

        /* data accessed via key points to allocated buffer */
        if ((ret = pthread_setspecific(key, buf)) != 0) {
            handle_error("pthread_setspecific", errno);
            free(buf);
            return NULL;
        }

        data = buf;
        printf("[tid:%d] TSD area addr: %p, alloced buf\n", i, data);
    }

    memcpy(data, "Hello, World!", strlen("Hello, World!") + 1);
    printf("[tid:%d]TSD area addr: %p, value:%s\n", i, data, (char *)data);

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
        if ((ret = pthread_create(&threads[i], NULL, func, (void *)(uintptr_t)j)) != 0)
            handle_error("pthread_create", errno);
    }

    for (i = 0; i < THREAD_NUM; i++)
        if ((ret = pthread_join(threads[i], NULL)) != 0)
            handle_error("pthread_join", errno);

    for (i = 0; i < THREAD_NUM; i++, j++) {
        /* uintptr_t guarantess that unsigned interger size is the same as the platform's */
        /*  1. cast j to uintptr_t so that the size of void * and j is the same (otherwise an error);
            2. cast j to void * to complie with the function signature (otherwise an error or a warning) */
        if ((ret = pthread_create(&threads[i], NULL, func, (void *)(uintptr_t)j)))
            handle_error("pthread_create", errno);
    }

    for (i = 0; i < THREAD_NUM; i++)
        if ((ret = pthread_join(threads[i], NULL)) != 0)
            handle_error("pthread_join", errno);

    (void)pthread_key_delete(key);

    return EXIT_SUCCESS;
}
