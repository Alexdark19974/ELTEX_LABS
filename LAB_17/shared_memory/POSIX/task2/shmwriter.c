#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SEGSIZE 4096

void server(void);

int main(void)
{
	server();

	exit(EXIT_SUCCESS);
} 

void server(void)
{
	int shmfd = shm_open("/shmwriter.c", O_CREAT | O_EXCL | O_RDWR, 0666);

	if (-1 == shmfd) {perror("shm_open"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

	int state = ftruncate(shmfd, SEGSIZE);

	if (-1 == state) {perror("ftruncate"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

	char *mmap_area_ptr = mmap(NULL, SEGSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);

	*mmap_area_ptr = '!';

	while (*mmap_area_ptr == '!')
	{
		printf("Server waiting for the changes in memory...\n");
	}

	printf("Finally...\n");
	sleep(5);

	state = munmap(mmap_area_ptr, SEGSIZE);

	if (-1 == state) {perror("munmap"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

	state = shm_unlink("/shmwriter.c");

	if (-1 == state) {perror("shm_unlink"); fprintf(stderr, "Error ocurred in %s, at line %d\n", __FILE__, __LINE__);}

}