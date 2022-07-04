#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#define SEG_SIZE 4096

int main(void)
{
    int shmfd = shm_open("/shmwriter.c", O_RDONLY, 0666);

    if (-1 == shmfd) {perror("shm_open"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    /*
     * Using ftruncate in the reader program will cause "bus error" since using shm_open() with flag "O_RDONLY"
     * Implies the abscence of memory size manipulation
     * If one wants to only read from memory, ftruncate() is prohibited.
     * */

 //    int state = ftruncate(shmfd, SEG_SIZE);
   
 //   if (-1 == state) {perror("ftruncate"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    char *msg_ptr = mmap(NULL, SEG_SIZE, PROT_READ, MAP_SHARED, shmfd, 0);

    /* 
     * shared mmaped area can only be read 
     * */

    if (NULL == msg_ptr) {perror("mmap"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}
    
    printf(" Message: %s from mmap area.\n", msg_ptr);

    /*
     * Deallocating mmaped memory and freeing the file descriptor associated with mmaped memory
     * Shared memory doesn't delete itself after the process is terminated.
     * */

    int state = munmap(msg_ptr, SEG_SIZE);

    if (-1 == state) {perror("munmap"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    state = shm_unlink("/shmwriter.c");

    if (-1 == state) {perror("shm_unlink"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

    exit(EXIT_SUCCESS);
}
