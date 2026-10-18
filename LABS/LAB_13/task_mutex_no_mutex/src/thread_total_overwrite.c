#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#define THREAD_NUM 2
#define INCR_MAX 100
#define BUF_SIZ 256
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)

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

        /* both threads may read args at the same time */
        tmp_ptr = (long *)args;
        /* but if thread 2 sets tmp to 0 by the time thread 1 finishes incrementing
            all the progress (100) will be lost: total overwrite case */
        tmp = *tmp_ptr;

        for (i = 0; i < INCR_MAX; i++) {
            /* both threads may increment tmp while having different values of it
                at the moment of overwriting the value */
            tmp++;
            /* both threads may store different values in tmp_ptr depending on which one is faster */
            *tmp_ptr = tmp;
            /* a = 1, then a = 2, then a = 1 again due to 2 threads writing different values and acting asynchronously */
            printf("a = %ld\n", *tmp_ptr);
        }

        return NULL;
}
