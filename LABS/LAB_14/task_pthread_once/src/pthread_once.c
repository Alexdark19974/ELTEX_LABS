#include <stdio.h>
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
    printf("addr %p: freed data\n", args);
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
    if (!args) {
        handle_error_msg("error: no thread id supplied");
        return NULL;        
    }
#endif

    i = *((int *)args);

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

    return NULL;
}

int main(void)
{
    int ret, i, j;
    int thread_ids[THREAD_NUM];
    pthread_t threads[THREAD_NUM];

    for (i = 0, j = 1; i < THREAD_NUM; i++, j++) {
        thread_ids[i] = j;
        if ((ret = pthread_create(&threads[i], NULL, func, (void *)&thread_ids[i])) != 0)
            handle_error("pthread_create", errno);
    }

    for (i = 0; i < THREAD_NUM; i++)
        if ((ret = pthread_join(threads[i], NULL)) != 0)
            handle_error("pthread_join", errno);

    for (i = 0; i < THREAD_NUM; i++, j++) {
        thread_ids[i] = j;
        if ((ret = pthread_create(&threads[i], NULL, func, (void *)&thread_ids[i])))
            handle_error("pthread_create", errno);
    }

    for (i = 0; i < THREAD_NUM; i++)
        if ((ret = pthread_join(threads[i], NULL)) != 0)
            handle_error("pthread_join", errno);

    (void)pthread_key_delete(key);

    return EXIT_SUCCESS;
}
