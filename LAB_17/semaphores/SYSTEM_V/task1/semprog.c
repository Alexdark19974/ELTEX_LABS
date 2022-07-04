#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#define SHM_SEG_SIZE 4096
#define INCR_MAX 10
#define TOP 20

void child (int, int *, struct sembuf *, struct sembuf *);

/* 
 * create a program where a a parent process and a child process increment a variable 'incr' in turn, using shared memorhy and semapohres.
 * They CANNOT increment value concurrently. They have to be serialized.
 * 
 * */

int main(void)
{
    int key_1 = ftok("semprog.c", 'A');

    if (-1 == key_1) {perror("ftok"); exit(EXIT_FAILURE);}

    int shmflags = IPC_CREAT | IPC_EXCL | 0666;
    int shmid = shmget(key_1, SHM_SEG_SIZE, shmflags);

    if (-1 == shmid) {perror("shmget"); exit(EXIT_FAILURE);}

    int key_2 = ftok("semprog.c", 'B');

    if (-1 == key_2) {perror("ftok"); exit(EXIT_FAILURE);}

    int nsems = 1;
    struct sembuf lock = {0, -1, 0}; // subtract 1 from semaphore 0
    struct sembuf unlock[2] = {{0, 0, 0}, {0, 1, 0}}; // add 1 if semval is 0
    int semflags = IPC_CREAT | IPC_EXCL| 0666;
    int semid = semget(key_2, nsems, semflags);

    if (-1 == semid) {perror("semget"); exit(EXIT_FAILURE);}

     union semun
    {
        int val;
        struct semid_ds *buf;
        unsigned short *array;
        struct seminfo *__buf;
    } arg;

    arg.val = 1;

    int state = semctl(semid, 0, SETVAL, arg);

    /* the value of semaphore 0 is set initialized to 1 to make the resource available */

    if (-1 == state) {perror("semctl"); exit(EXIT_FAILURE);}

    int *shm_incr_ptr = shmat(shmid, NULL, 0);

    if (-1 == *shm_incr_ptr) {perror("shmat"); exit(EXIT_FAILURE);}

    int cpid_1 = 0;
    int cpid_2 = 0;

    cpid_1 = fork();

    if (-1 == cpid_1) {perror("fork"); exit(EXIT_FAILURE);}

    if (0 == cpid_1)
    {
        child(semid, shm_incr_ptr, &lock, unlock);
    }

    cpid_2 = fork();

    if (-1 == cpid_1) {perror("fork"); exit(EXIT_FAILURE);}

    if (0 == cpid_2)
    {
        child(semid, shm_incr_ptr, &lock, unlock);
    }

    int wstatus = wait(&cpid_1);

    if (-1 == wstatus) {perror("wait"); exit(EXIT_FAILURE);}

    wstatus = wait(&cpid_2);

    if (-1 == wstatus) {perror("wait"); exit(EXIT_FAILURE);}

    printf("the total result of incrementation is %d\n", *shm_incr_ptr);

    struct semid_ds buf_1;

    int semstatus = semctl(semid, nsems , IPC_RMID, &buf_1);

    if (-1 == semstatus) {perror("semctl"); exit(EXIT_FAILURE);}

    struct shmid_ds buf_2;

    int shmstatus = shmdt(shm_incr_ptr);

    if (-1 == shmstatus) {perror("shmctl"); exit(EXIT_FAILURE);}

    shmstatus = shmctl(shmid, IPC_RMID, &buf_2);

    if (-1 == shmstatus) {perror("shmctl"); exit(EXIT_FAILURE);}
    exit(EXIT_SUCCESS);
}

void child(int semid, int *shm_incr_ptr, struct sembuf *lock, struct sembuf *unlock)
{
    int semops = 0;
    int status = 0;

    while (*shm_incr_ptr < TOP)
    {
        semops = 1; 

        status = semop(semid, lock, semops);

        /* the semaphore will block if the semval is 0, since the value of the semaphore cannot be less than 0 */
        /* An attempt to subtract -1 when the semval is 0 will result in being blocked */
        /* START CRITICAL SECTION */

        if (-1 == status) {perror("semops"); exit(EXIT_FAILURE);}

        if (*shm_incr_ptr >= TOP)
        {
            break;
        }

        int temp_incr = 0;

        while (temp_incr++ < INCR_MAX)
        {
            *shm_incr_ptr = *shm_incr_ptr + 1;
            printf("Incremented at %d by child (PID: %d), parent - (PPID: %d)\n", *shm_incr_ptr, getpid(), getppid());
            sleep(1);
        }

        semops = 2;

        status = semop(semid, unlock, semops);

        /* END CRITCIAL SECTION */

        /* Adding 1 back to semaphore 0, thus making the resource available again */

        if (-1 == status) {perror("semops"); exit(EXIT_FAILURE);}

        sleep(5);
    }

    int shmstatus = shmdt(shm_incr_ptr);

    if (-1 == shmstatus) {perror("shmdt"); exit(EXIT_FAILURE);}

    printf("exiting\n");

    exit(EXIT_SUCCESS);
}

