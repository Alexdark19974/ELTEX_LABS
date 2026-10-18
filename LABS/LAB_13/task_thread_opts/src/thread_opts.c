#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#define BUF_SIZ 256
#define handle_error(msg, errnum)  do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = strerror_r((errnum), buf, sizeof(buf)); \
                                    fprintf(stderr, "[%s:%d] error: %s: %s", __func__, __LINE__, (msg), err_str); \
                                } while(0)

struct thread_info {    /* Used as argument to thread()  */
   pthread_t thread_id; /* ID returned by thread_create() */
   int thread_num;      /* Application-defined thread # */
   char *argv_string;   /* From command-line argument */
};

/* Thread start funciton: display address near top of our stack,
   and return upper-cased copy of argv_string */
static void *thread_start(void *arg)
{
   struct thread_info *tinfo = NULL;
   char *uargv = NULL;
   char *p = NULL;

   if (!arg) {
      fprintf(stderr, "[%s:%d] error: arg is not supplied for thread\n", __func__, __LINE__);      
      return NULL;      
   }

   tinfo = arg;

   printf("Thread %d: top of stack near %p; argv_string = %s\n", tinfo->thread_num, (void *)&tinfo, tinfo->argv_string);

   if (!(uargv = strdup(tinfo->argv_string))) {
      handle_error("strdup", errno);
      return NULL;
   }

   for (p = uargv; *p != '\0'; p++) {
      *p = toupper(*p);
   }

   return uargv;
}

/* The "-s" option specifies a stack size for our threads
   Followed by arguments, the total number of which is set via thread_num */
int main(int argc, char *argv[])
{
   int ret = 0, tnum = 0;
   int opt = 0, num_threads = 0;
   size_t stack_size = 0;
   pthread_attr_t attr = {0};
   struct thread_info *tinfo = NULL;
   void *retp = NULL;

   /* Initialize thread creation attributes */
   if ((ret = pthread_attr_init(&attr)) != 0) {
      handle_error("pthread_attr_init", errno);
      goto err;
   }

   if ((ret = pthread_attr_getstacksize(&attr, &stack_size)) != 0) {
         handle_error("pthread_attr_getstacksize", errno);
         goto err;
   }

   printf("main thread stack_size=%zu\n", stack_size);
   stack_size = 0;

   printf("optind = %d\n", optind);
   /* optind is 3 after processing (1) -s (2) 32768, which is argv[3] */
   while ((opt = getopt(argc, argv, "s:")) != -1) {
      printf("optind = %d\n", optind);
      switch (opt) {
         case 's':
            stack_size = strtoul(optarg, NULL, 0);
            if ((long int)stack_size < PTHREAD_STACK_MIN) {
               fprintf(stderr, "[%s:%d] error: stack_size = %zu, minimum is %ld\n", __func__, __LINE__, stack_size, PTHREAD_STACK_MIN);
               goto err;
            }
            break;
         default:
            fprintf(stderr, "Usage: %s [-s stack-size] arg...\n", argv[0]);
            goto err;
      }
   }

   printf("argc = %d, optind = %d\n", argc, optind);

   /* Allocate memory for pthread_create() arguments */
   num_threads = argc - optind;
   printf("requested stack_size=%zu\n", stack_size);

   if (stack_size > 0 &&
      (ret = pthread_attr_setstacksize(&attr, stack_size)) != 0) {
         handle_error("pthread_attr_setstacksize", errno);
         goto err;
   }

      printf("num_threads = %d\n", num_threads);
   if (!(tinfo = calloc(num_threads, sizeof(struct thread_info)))) {
      handle_error("calloc", errno);
      goto err;
   }

   /* Create one thread for each command-line argument */
   for (tnum = 0; tnum < num_threads; tnum++)
   {
      tinfo[tnum].thread_num = tnum;
      tinfo[tnum].argv_string = argv[optind + tnum];

      /* The pthread_create() call inits thread_id, passes tinfo element to thread_start */
      if ((ret = pthread_create(&tinfo[tnum].thread_id, &attr, &thread_start, &tinfo[tnum])) != 0)
         handle_error("pthread_create", errno);
   }

   /* Destroy the thread attributes object, since it is no longer needed */

   if ((ret = pthread_attr_destroy(&attr)) != 0)
      handle_error("pthread_attr_destroy", errno);

   /* Now join with each thread, and display its returned value */

   for (tnum = 0; tnum < num_threads; tnum++) {
      if ((ret = pthread_join(tinfo[tnum].thread_id, &retp)) != 0) {
         handle_error("pthread_join", errno);
      } else
         printf(" thread %d; returned value was %s\n", tinfo[tnum].thread_num, (char *)retp);
      free(retp); /* Free memory allocated by thread */
      retp = NULL;
   }

err:
   free(tinfo);
   return EXIT_SUCCESS;
}

