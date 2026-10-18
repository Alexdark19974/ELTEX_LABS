#include "chatroom_header.h"

int shm_mem_init(char *shmmem_name)
{
    int shmfd = shm_open(shmmem_name, O_CREAT | O_RDWR, 0666);
    error_check(shmfd, "shm_open");
    int state = ftruncate(shmfd, SEGSIZE);
    error_check(state, "ftruncate");
    
    return shmfd;
}
