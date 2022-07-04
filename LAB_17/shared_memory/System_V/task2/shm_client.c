#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHM_SEG_SIZE 4096

int main(void)
{
    int key = ftok("shm_server.c", 'B');

    if (-1 == key) {perror("ftok"); exit(EXIT_FAILURE);}

    int shmid = shmget(key, SHM_SEG_SIZE, 0);

    if (-1 == shmid) {perror("shmget"); exit(EXIT_FAILURE);}

    char *str = shmat(shmid, NULL, 0);

    if (-1 == *str) {perror("shmat"); exit(EXIT_FAILURE);}

    char symbol = 0;

    printf("Enter any char: ");
    fflush(stdout);
    sync();

    int state = read(STDIN_FILENO, &symbol, sizeof(char));

    if (-1 == state) {perror("read"); exit(EXIT_FAILURE);}
    
    state = snprintf(str, 2, "%c", symbol);

    if (-1 == state) {perror("snprintf"); exit(EXIT_FAILURE);}

    state = shmdt(str);

    if (-1 == state) {perror("shmdt"); exit(EXIT_FAILURE);}

    exit(EXIT_SUCCESS);
}
