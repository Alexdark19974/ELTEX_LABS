#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#define THREAD_NUM 10
#define INCR_MAX 100000000
#define BUF_SIZ 256
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *thread_increment(void *args);

int main(void)
{
        long a = 0; // shared resource
        int i = 0, ret = 0;
        pthread_t threads[THREAD_NUM] = {0};

        for (i = 0; i < THREAD_NUM; i++) {
            /* 2 threads start asynchronously, modifying arg a */
            if ((ret = pthread_create(&threads[i], NULL, &thread_increment, &a)) != 0)
                handle_error("pthread_create", errno);
        }

        for (i = 0; i < THREAD_NUM; i++) {
            /* main thread waits for each thread to terminate */
            if ((ret = pthread_join(threads[i], NULL)) != 0)
                handle_error("pthread_join", errno);
        }

        printf("the result of %d threads doing %d increments is %ld\n", THREAD_NUM, INCR_MAX, a);

        return EXIT_SUCCESS;
}

void *thread_increment(void *args)
{
        long *tmp_ptr = NULL;
        long tmp = 0;
        int i = 0;

        if (!args)
            return NULL;

        tmp_ptr = (long *)args;

        for (i = 0; i < INCR_MAX; i++) {
            tmp++;
        }

        /* Local accumulation approach */
        (void)pthread_mutex_lock(&mutex);
        *tmp_ptr += tmp;
        printf("a = %ld\n", *tmp_ptr);
        /* We let threads incrementing their local tmp in parallel, 
            And only apply mutex to merge tmp with *tmp_ptr in synch */
        (void)pthread_mutex_unlock(&mutex);
        /* There are only 10 locks and locks in total */

        return NULL;
}
