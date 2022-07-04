#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define MEM_SEG_SIZE 1000

int main(void)
{
    char buffer[100] = {0};


    int state = write(STDOUT_FILENO, "Enter the string: ", 19);

    if (-1 == state)
    {
        perror("write");
        exit(EXIT_FAILURE);
    }

    state = read(STDIN_FILENO, buffer, 100);

    if (-1 == state)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }


    int length = strlen(buffer);

    int key = ftok("shm_writer.c", 'A');

    if (-1 == key)
    {
        perror("smget");
        exit(EXIT_FAILURE);
    }

    int shmid = shmget(key, MEM_SEG_SIZE, IPC_CREAT | 0666); // allocates shared memory returning SHMID - shared memory identifier

    if (-1 == shmid)
    {
        perror("smget");
        exit(EXIT_FAILURE);
    }

    char *str = shmat(shmid, NULL, 0); //attaches the base address of shared memory.

    strncpy(str, buffer, length);
    str[length - 1] = '\0';

    printf(" %s is in shared memory section, (addr: %p).\n", str, (void *) &str[0]);

    state = shmdt(str);

    if (-1 == state)
    {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

