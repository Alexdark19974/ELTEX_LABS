#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> 
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#define SEGSIZE 4096

void client(void);

int main(void)
{
	client();

	exit(EXIT_SUCCESS);
}

void client(void)
{
	int shmfd = shm_open("/shmwriter.c", O_RDWR, 0666);

	if (-1 == shmfd) {perror("shm_open"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

	char *mmap_area_ptr = mmap(NULL, SEGSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

	printf(" Press any button to stop the server: ");
	int state = fgetc(stdin);

	*mmap_area_ptr = state;

	if (-1 == state) {perror("fgetc"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

	state = munmap(mmap_area_ptr, SEGSIZE);

	if (-1 == state) {perror("munmap"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

}