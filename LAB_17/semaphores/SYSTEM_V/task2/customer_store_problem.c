#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#define TRUE 1
#define randomizer_1() (rand() % 2)
/*
 *
 * Customer 1 = 5000 units.
 * Customer 2 = 5100 units.
 * Customer 3 = 4500 units.
 * Store 1 = 1000
 * Store 2 = 950
 * Supplier = + 500 units.
 * A customer takes all units from Store.
 *
 * If done right, no deadlock will occur.
 *
 * */

void stores_customers_init(void);
void customer(int, int, int *, struct sembuf *, struct sembuf *);
void supplier (int, int *, struct sembuf *, struct sembuf *);

int main(void)
{
    stores_customers_init();

    exit(EXIT_SUCCESS);
}

void stores_customers_init(void)
{
    int key_1 = ftok("customer_store_problem.c", 'A');

    if (-1 == key_1) {perror("ftok"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}
    
    int size = 4096;
    int shmflg = IPC_CREAT | IPC_EXCL | 0666;
    struct shmid_ds buf;
    int shmid = shmget(key_1, size, shmflg);

    if (-1 == shmid) {perror("shmget"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    int *data_ptr = shmat(shmid, NULL, 0);

    /*
     * Using shared memory is reduntant in this program but it demonstrates
     * the work of shared memory and semaphores nicely.
     * */

    if (-1 == *data_ptr) {perror("shmat"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    data_ptr[0] = 1000; // initialize Store 1
    data_ptr[1] = 950;  // initialize Store 2
    data_ptr[2] = 5000; // initialize Customer 1
    data_ptr[3] = 5100; // initialize Customer 2
    data_ptr[4] = 4500; // initialize Customer 3
    data_ptr[5] = 500;  // initialize Supplier

    int key_2 = ftok("customer_store_problem.c", 'B');

    if (-1 == key_2) {perror("ftok"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    int nsems = 2;
    int semflg = IPC_CREAT | IPC_EXCL | 0666;
    int semid = semget(key_2, nsems, semflg);

    if (-1 == semid) {perror("semget"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    struct sembuf lock[2] = {{0, -1, 0}, {1, -1, 0}}; 

    /* initialize lock[2] for semaphores 1 & 2 to subtract the value 
     * thus making the the resource unavailable */

    struct sembuf unlock[4] = {{0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0}}; 

    /* initialize unlock[4] for semahores 1 & 2 to enable adding the value back 
     * and check both if the val is 0. 
     * if true, then increment the val, thus making the resource available again. */

    union semun
    {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
        struct seminfo *__buf;
    } arg;
    arg.val = 1;

    int state = semctl(semid, 0, SETVAL, arg);

    if (-1 == state) {perror("semctl"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    state = semctl(semid, 1, SETVAL, arg);

    if (-1 == state) {perror("semctl"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    srand(0);

    int cstm_pid_1 = 0;
    int cstm_pid_2 = 0;
    int cstm_pid_3 = 0;
    int supplier_pid_3 = 0;

    /* 3 child-processes perform their purchasing function, while the 4-th child runs independently */

    cstm_pid_1 = fork();

    if (-1 == cstm_pid_1) {perror("fork"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    if (0 == cstm_pid_1)
    {
        customer(semid, 2, data_ptr, lock, unlock);
    }

    cstm_pid_2 = fork();

    if (-1 == cstm_pid_2) {perror("wait"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    if (0 == cstm_pid_2)
    {
        customer(semid, 3, data_ptr, lock, unlock);
    }

    cstm_pid_3 = fork();

    if (-1 == cstm_pid_3) {perror("wait"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    if (0 == cstm_pid_3)
    {
        customer(semid, 4, data_ptr, lock, unlock);
    }

    supplier_pid_3 = fork();

    if (-1 == supplier_pid_3) {perror("wait"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    if (0 == supplier_pid_3)
    {
        supplier(semid, data_ptr, lock, unlock);
    }

    /* Parent process waits untill all children finish their work */

    int wstatus = wait(&cstm_pid_1);

    if (-1 == wstatus) {perror("wait"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    wstatus = wait(&cstm_pid_2);

    if (-1 == wstatus) {perror("wait"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    wstatus = wait(&cstm_pid_3);

    if (-1 == wstatus) {perror("wait"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    /* Kill the 4th child since it doesn't have exit condition */

    wstatus = kill(supplier_pid_3, SIGTERM);

    printf("Killed the supplier.\n");

    sleep(2);

    /* Deallocate resources and clean the memory */

    state = shmdt(data_ptr);

    if (-1 == state) {perror("shmdt"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    state = shmctl(shmid, IPC_RMID, &buf);

    if (-1 == state) {perror("shmctl"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    state = semctl(semid, 0 , IPC_RMID);

    if (-1 == state) {perror("semctl"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}
    
    state = semctl(semid, 1, IPC_RMID);

    if (-1 == state) {perror("semctl"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    printf (" Every customer has been satisified.\n");
}

/*
 * Only 1 process can be at the store, so, provided that 2 different processes are at 2 different stores
 * The 3rd process will block on the semaphore, since semaphore 1 and 2 have been taken over.
 *
 * If 2 process are trying to take over the same semaphore, only one of them will pass.
 * The other one will block until the resource is released.
 * */

void customer(int semid, int customer_nmbr, int *data_ptr, struct sembuf *lock, struct sembuf *unlock)
{
    int cstmn = customer_nmbr;
    int nsops = 0;

    while (data_ptr[cstmn] > 0)
    {
        int strmn = randomizer_1();

        printf("Customer %d with demand %d at store %d purchasing goods...\n", cstmn - 1, data_ptr[cstmn], strmn);

        nsops = 1;

        semop(semid, lock + strmn, nsops);

        /* 
         * semop() locks the chosen semaphore by subtracting 1, thus making it 0
         * Any attempt to go below 0 results in the semaphore getting blocked.
         * */

        /* START CRITICAL SECTION */

        if (0 == data_ptr[strmn])
        {
            printf(" No goods at store %d...\n", strmn);
        }
        else
        {
            /* 
             * Clearing out the chosen store , assigning 0 to the store *
             */

            data_ptr[cstmn]-= data_ptr[strmn];
            data_ptr[strmn] = 0;
        }

        nsops = 2;

        /* 
         * Depending on which store number (strmn = 0 or 1) is chose, 
         * Release the resource and make it available for the next process to take it over
         * If done incorrectly or lacking condition down below, releasing the wrong semaphore
         * Will lead to a deadlock situation
         * */

        if (1 == strmn)
        {
            semop(semid, unlock + 2, nsops);
        }
        else
        {
            semop(semid, unlock, nsops);
        }

        /* 
         * Incrementing by 1 leads to the resource (the n-th semaphore) being available again.
         * 
         * */

        /* END CRITICAL SECTION */

        printf("Customer %d with demand %d at store %d finished purcashing...\n", cstmn - 1, data_ptr[cstmn], strmn);

        sleep(2);
    }

    shmdt(data_ptr);

    printf("Customer %d is satisfied. Leaving...\n", cstmn - 1);

    exit(EXIT_SUCCESS);
}

/* 
 * Occasionally, the supplier-process steps in and takes over one of 2 semaphores, thus
 * Blocking the customer-processes since there's resource competition.
 * Only 2 semaphores are present while there are 3 customers and 2 stores.
 * */ 

void supplier (int semid, int *data_ptr, struct sembuf *lock, struct sembuf *unlock)
{ 
    int nsops = 0;

    while (TRUE)
    {
        int strmn = randomizer_1();
        printf (" Delivering supplies to store %d.\n", strmn);
        nsops = 1;

        semop(semid, lock + strmn, nsops);

        /* START CRITICAL SECTION */

        data_ptr[strmn]+= data_ptr[5];

        /* 
         * increment the number of units at a chosen store by 500 units (is located at data_ptr[5])
         * */

        nsops = 2;

        if (1 == strmn)
        {
            semop(semid, unlock + 2, nsops);
        }
        else
        {
            semop(semid, unlock, nsops);
        }

        /*
         * Unlock the resource and mind the right semaphore 
         * */

        /* END CRITICAL SECTION */

        printf (" Delivered supplies to store %d.\n", strmn);

        sleep(5);
    }
}
