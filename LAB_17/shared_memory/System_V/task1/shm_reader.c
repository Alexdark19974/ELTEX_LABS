#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHM_SEG_SIZE 1000

int main(void)
{
    int key = ftok("shm_writer.c", 'A');

    if (-1 == key)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    struct shmid_ds buf;

    int shmid = shmget(key, SHM_SEG_SIZE, 0);

    if (-1 == shmid)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    char *str = shmat(shmid, NULL, SHM_RDONLY);

    if (-1 == *str)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    printf("Shared memory segment: %s (addr: %p)", str, (void *) &str[0]); // virtual address is different than shm_writer.c, but it is okay since both point to the the same physical address.

    int state = shmdt(str);

    if (-1 == state)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    state = shmctl(shmid, IPC_RMID, &buf);

    if (-1 == state)
    {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
