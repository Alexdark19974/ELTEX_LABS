#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#define MAX_THREAD 4
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
        do { perror(msg); fprintf(stderr, "error happened in file %s, line %d", __FILE__, __LINE__); exit(EXIT_FAILURE); } while (0)

typedef struct Thread
{
    pthread_t thread_id;
    int thread_num;
} Thread;

pthread_once_t once = PTHREAD_ONCE_INIT;
pthread_key_t key;

void once_func(void);

void destroyer(void *const args)
{
    printf("terminated thread-unique storage.\n");
    free(args);
}

void make_key(void)
{
    int state = 0;
    state = pthread_key_create(&key, destroyer);

    if (0 != state)
    {
	handle_error_en(state, "pthread_key_create");
    }	    
}

void *func(void *const args)
{
    int state = 0;
    void *ptr = NULL;
    state = pthread_once(&once, &make_key);

    if (0 != state)
    {
        handle_error_en(state, "pthread_once");
    }
    while (1)
    {
        if ((ptr = pthread_getspecific(key)) == NULL)
    	{
            ptr = malloc(BUFSIZ);
            state = pthread_setspecific(key, ptr);
	    
	    if (0 != state)
            {
                handle_error_en(state, "pthread_setspecific");
	    }
        }
        else
        {
            printf(" storage area: %p\n", ptr);
            char *func_ptr = ptr;
	    printf("%p\n", func_ptr);
	    char *string = "Hello, World!\n";
            func_ptr = strdup(string);
	   // strcpy(func_ptr, string);
	    printf("%p\n", func_ptr);
            printf("%s\n", func_ptr);
	    sync();
           // sleep(3);
	    break;
        }
    }

    pthread_exit(NULL);
}

int main(void)
{
    once_func();

    exit(EXIT_SUCCESS);
}

void once_func(void)
{
    int state = 0;

    struct Thread *thread_ptr = calloc(MAX_THREAD, sizeof(struct Thread));

    if (NULL == thread_ptr)
    {
        handle_error("calloc");
    }

    for (int i = 0; i < MAX_THREAD; i++)
    {
        state = pthread_create(&thread_ptr[i].thread_id, NULL, &func, NULL);

        if (0 != state)
        {
            handle_error_en(state, "pthread_create");
        }
    }

    for (int i = 0; i < MAX_THREAD; i++)
    {
        state = pthread_join(thread_ptr[i].thread_id, NULL);
	sleep(3);

        if (0 != state)
        {
            handle_error_en(state, "pthread_join");
        }
    }

    for (int i = 0; i < MAX_THREAD; i++)
    {
        state = pthread_create(&thread_ptr[i].thread_id, NULL, &func, NULL);

        if (0 != state)
        {
            handle_error_en(state, "pthread_create");
        }
    }

    for (int i = 0; i < MAX_THREAD; i++)
    {
        state = pthread_join(thread_ptr[i].thread_id, NULL);
        sleep(3);

        if (0 != state)
        {
            handle_error_en(state, "pthread_join");
        }
    }

    free(thread_ptr);
    thread_ptr = NULL;
}
