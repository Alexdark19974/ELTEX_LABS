#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SHM_SEG_SIZE 4096
int main(void)
{
    int key = ftok("shm_server.c", 'B');

    if (-1 == key)
    {
        perror("ftok");
        exit(EXIT_SUCCESS);
    }

    int shmid = shmget(key, SHM_SEG_SIZE, IPC_CREAT | 0666);

    if (-1 == shmid)
    {
        perror("shmget");
        exit(EXIT_SUCCESS);
    }

    char *str = shmat(shmid, NULL, 0);

    str[0] = '!';

    while (*str == '!')
    {
        printf("Server waiting for changes in memseg...\n");
        sleep(2);
    }

    printf("Hallelujah!\n");

    sleep(2);

    printf("Displaying stat onto screen.\n\n");

    sleep(2);

    struct shmid_ds buf;

    int state = shmdt(str);

    if (-1 == state)
    {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    state = shmctl(shmid, IPC_STAT, &buf);

    if (-1 == state)
    {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    printf("Key supplied to shmget(): %d\n", buf.shm_perm.__key);
    printf("Effective UID of owner: %d\n", buf.shm_perm.uid);
    printf("Effective GID of owner: %d\n", buf.shm_perm.gid);
    printf("Effective UID of creator: %d\n", buf.shm_perm.cuid);
    printf("Effective GID of creator: %d\n", buf.shm_perm.cgid);
    printf("Permissions +  SHM_DEST and SHM_LOCKED flags: %d\n", buf.shm_perm.mode);
    printf("Sequence number: %d\n", buf.shm_perm.__seq);
    printf("Size of segments (bytes): %ld\n", buf.shm_segsz);
    printf("Last attach time: %ld\n", buf.shm_atime);
    printf("Last detach time: %ld\n", buf.shm_dtime);
    printf("Last change time: %ld\n", buf.shm_ctime);
    printf("PID of creator: %d\n", buf.shm_cpid);
    printf("PID of last shmat()/shmdt(): %d\n", buf.shm_lpid);
    printf("Number of current attaches: %ld\n", buf.shm_nattch);

    state = shmctl(shmid, IPC_RMID, &buf);

    if (-1 == state)
    {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
