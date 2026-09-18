#include "chatroom_header.h"

void cleanup(struct Garbage_collector *gb)
{
    int state = munmap(gb->shm_ipc_ptr_1, SEGSIZE);
    error_check(state, "munmap");

    state = shm_unlink(gb->shm_name_1_ptr);
    error_check(state, "shm_unlink");
 
    state = sem_close(gb->sem_ptr_1);
    error_check(state, "sem_close");

    state = sem_close(gb->sem_ptr_2);
    error_check(state, "sem_close");

    state = sem_unlink(gb->sem_name_ptr_1);
    error_check(state, "sem_unlink");

    state = sem_unlink(gb->sem_name_ptr_2);
    error_check(state, "sem_unlink");

    free(gb->sem_name_ptr_1);
    gb->sem_name_ptr_1 = NULL;

    free(gb->sem_name_ptr_2);
    gb->sem_name_ptr_2 = NULL;

    free(gb->ptr);
    gb->ptr = NULL;

    free(gb->shm_name_1_ptr);
    gb->shm_name_1_ptr = NULL;
}

