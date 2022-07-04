#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#define SHM_SEG_SIZE 4096
#define MSG_SIZE 100
int main(void)
{
    int shmfd = shm_open("/shmwriter.c", O_CREAT | O_EXCL | O_RDWR, 0666);

    if (-1 == shmfd) {perror("shm_open"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    int state = ftruncate(shmfd, SHM_SEG_SIZE);

    if (-1 == state) {perror("ftruncate"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    char msg[100] = {0};
    char *check_ptr = NULL;

    printf("Enter the message: ");

    /* Not necessary to try to sync.
     * Do I not need to use fflush(stdout) and sync()
     * Because fgets() dumps the content of the library buffer in advance?
     * */

    check_ptr = fgets(msg, MSG_SIZE, stdin);

    if (NULL == check_ptr) {perror("fgets"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    char *mmap_area_ptr = mmap(NULL, SHM_SEG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

    strncpy(mmap_area_ptr, msg, strlen(msg) - 1);

    mmap_area_ptr[strlen(msg)] = '\0';

    printf("string %s was copied into mmap shared area.\n", mmap_area_ptr);

    state = munmap(mmap_area_ptr, SHM_SEG_SIZE);

    if (-1 == state) {perror("munmap"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    exit(EXIT_SUCCESS);
}

