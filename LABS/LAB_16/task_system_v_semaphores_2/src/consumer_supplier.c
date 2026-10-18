#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#define RETURN (1)
#define CONSUME (-1)
#define DEFAULT_PATHNAME "/proc/meminfo"
#define MAGIC_ID 'A'
#define MAGIC_ID2 'B'
#define MAGIC_ID3 'C'
#define CONSUMER_OPT 'c'
#define SUPPLIER_OPT 's'
#define DEMAND_OPT 'D'
#define SUPPLY_OPT 'S'
#define HELP_OPT 'h'
#define BUF_SIZ 256
#define PERMS 0666 // -rw-rw-rw
#define KEY_NUM 3
#define JOBS_NSEMS_NUM 1
#define SUPPLY_UNIT 500
#define DEMAND_UNIT 5000
#define handle_error(msg, errnum) do { \
                                    char buf[BUF_SIZ] = {0}; \
                                    char *err_str = NULL; \
                                    if (!(err_str = strerror_r((errnum), buf, sizeof(buf)))) \
                                        fprintf(stderr, "error: failed to print error message!\n"); \
                                    else \
                                        fprintf(stderr, "[%s:%d] error: %s: %s\n", __func__, __LINE__, (msg), err_str); \
                                } while(0)
#define handle_error_msg(fmt, ...) do { \
                            fprintf(stderr, fmt, __VA_ARGS__); \
                        } while(0)
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

typedef struct supplier_s {
    int sup_unit;
} supplier_t;

typedef struct consumer_s {
    int dem_unit;
} consumer_t;

void print_usage(const char *prog_name);
bool options_bundled(int opt, const char *optarg);
int consumer_supplier(int argc, char **argv);
void consume(int shmid, int semid, int jobs_semid, const int sup_num, const int cons_idx, const int cons_num);
void supply(int shmid, int semid, int jobs_semid, const int cons_num, const int sup_idx, const int sup_num, const int restock_amount);

/* Only 1 consumer must be able to take the shared resource which is a set of entities: a supplier, its supply_unit and semaphore.
   The consumer grabs the lock, thus entering BUSY/OCCUPIED state and preventing anyone else from accessing the shared resource.
   The rest of consumers block on semop - an attempt to subtract 1 from semval=0, which is impossible by design.
   On blocking, a consumer enters the PENDING state if 2 consumers and/or a supplier subtract from the same semaphore.
   After one consumer has taken the supply unit, it goes to release the lock.
   Releasing the lock is an addition to semval=0, thus making the resource available. semval=0 is a BUSY/OCCUPIED state, semval=1 is an AVAILABLE state.
   Adding to semval is done by unlock.
   Then the consumer sleeps allowing a supplier and/or any other consumer enter a racing condition.
   The racing condition is an attempt to re-acquire the lock, i.e to do the subtraction and enter the BUSY/OCCUPIED state (AVAILABLE / RELEASED STATE).
   The process is awakened from sleep() or unblocked on semop(), which is the kernel scheduler's job.
   jobs is also a part of the shared resource. The number of jobs is a number of consumers.
   jobs servers as a flag to denote the termination of all consumers. It has its own semaphore, and 2 processes cannot modify it at the same time.

   Trivia:
   1) Number of locks/unlocks = number of suppliers = number of shared resources;
   2) Processes work with shared memory mapped to consumers or suppliers, each having its own address spaces within the same shared memory block;
   3) The shared resource (i.e, a supply_unit) cannot be modified by 2 processes - consumers or suppliers - at the same time;
   4) rand() % sup_num enables random semaphore's selection: it is possible for 2 processes to get the same semaphore resulting in a race condition.
   5) 2 processes can independently grab different semaphores if sup_idx and lock_idx are different;
   6) A single supplier has its own semaphore represented by its lock/unlock. The supplier cannot select a semaphore;
   7) sleep() gives each supplier and consumer a fighting chance to win in the race condition, i.e get access to the shared resource;
   8) A consumer must release its taken semaphore on satisfaction or critical error as well as detach the shared memory segment;
   9) A satisfied consumer is one whose demand has been reduced to 0;
   10) When all consumers leave, the parent process kills suppliers, deallocates all resources and leaves;
   11) The totality of shared data: supplier_t + consumer_t + int */

int main(int argc, char **argv)
{
    return consumer_supplier(argc, argv);
}

int consumer_supplier(int argc, char **argv)
{
    const char *shmem_file = NULL, *prog_name = NULL, magic[KEY_NUM] = {MAGIC_ID, MAGIC_ID2, MAGIC_ID3};
    void *memseg = NULL;
    pid_t *pid_list = NULL;
    consumer_t *consumer = NULL;
    supplier_t *supplier = NULL;
    int *jobs = NULL;
    size_t memseg_size = 0, cons_size = 0, sup_size = 0;
    long num = 0;
    struct stat sb = {0};
    union semun arg;
    int opt = 0, wret = 0, status = EXIT_SUCCESS;
    int cons_num = 0, sup_num = 0, proc_num = 0, supply_unit = 0, demand_unit = 0;
    int shmid = -1, semid = -1, jobs_semid = -1, key[KEY_NUM] = {-1};
    int nsems, i, pidx;

    prog_name = argv[0];

    while ((opt = getopt(argc, argv, ":hs:c:D:S:")) != -1) {
        switch (opt) {
            case CONSUMER_OPT:
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                errno = 0;
                num = strtol(optarg, NULL, 10);
                if (errno != 0) {
                    handle_error("strtol", errno);
                    status = EXIT_FAILURE;
                    goto print;
                } else if (num <= 0) {
                    handle_error_msg("Number of consumers must be a strictly positive integer. Your number is %ld\n", num);
                    goto print;
                }
                cons_num = (int)num;
                break;
            case SUPPLIER_OPT:
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                errno = 0;
                num = strtol(optarg, NULL, 10);
                if (errno != 0) {
                    handle_error("strtol", errno);
                    status = EXIT_FAILURE;
                    goto print;
                } else if (num <= 0) {
                    handle_error_msg("Number of suppliers must be a strictly positive integer. Your number is %ld\n", num);
                    goto print;
                }
                sup_num = (int)num;
                break;
            case DEMAND_OPT:
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                errno = 0;
                num = strtol(optarg, NULL, 10);
                if (errno != 0) {
                    handle_error("strtol", errno);
                    status = EXIT_FAILURE;
                    goto print;
                } else if (num <= 0) {
                    handle_error_msg("Demand unit must be a strictly positive integer. Your number is %ld\n", num);
                    goto print;
                }
                demand_unit = (int)num;
                break;
            case SUPPLY_OPT:
                if (!optarg || true == options_bundled(opt, optarg))
                    goto print;
                errno = 0;
                num = strtol(optarg, NULL, 10);
                if (errno != 0) {
                    handle_error("strtol", errno);
                    status = EXIT_FAILURE;
                    goto print;
                } else if (num <= 0) {
                    handle_error_msg("Supply unit must be a strictly positive integer. Your number is %ld\n", num);
                    goto print;
                }
                supply_unit = (int)num;
                break;
            case HELP_OPT:
                goto print;
            case ':':
                if (optopt == CONSUMER_OPT || optopt == SUPPLIER_OPT || optopt == DEMAND_OPT || optopt == SUPPLY_OPT)
                    handle_error_msg("-%c requires an argument, e.g. 4\n", optopt);
                goto print;
            default: // '?'
                handle_error_msg("-%c is an unknown option\n", optopt);
                goto print;
        }
    }

    if (!cons_num) {
        handle_error_msg("Number of consumers is not specified. Use '-%c' option!\n", CONSUMER_OPT);
        goto print;
    } else if (!sup_num) {
        handle_error_msg("Number of suppliers is not specified. Use '-%c' option!\n", SUPPLIER_OPT);
        goto print;
    }

    if (!demand_unit) {
        printf("Demand unit is not specified. Defaulting to %d\n", DEMAND_UNIT);
        demand_unit = DEMAND_UNIT;
    }

    if (!supply_unit) {
        printf("Supply unit is not specified. Defaulting to %d\n", SUPPLY_UNIT);
        supply_unit = SUPPLY_UNIT;
    }

    shmem_file = prog_name; // program name is an accessible file
    if (stat(shmem_file, &sb) < 0) {
        handle_error("stat", errno);
        status = EXIT_FAILURE;
        goto end;        
    }

    proc_num = cons_num + sup_num;
    if (!(pid_list = calloc((size_t)proc_num, sizeof(pid_t)))) {
        handle_error("calloc", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    for (i = 0; i < KEY_NUM; i++) {
        if ((key[i] = ftok(shmem_file, magic[i])) < 0) {
            handle_error("ftok", errno);
            status = EXIT_FAILURE;
            goto end;
        }
    }
    i = 0;

    cons_size = cons_num * sizeof(consumer_t); // size of consumers
    sup_size = sup_num * sizeof(supplier_t); // size of suppliers
    memseg_size = cons_size + sup_size + sizeof(int); // calculate size of jobs_num. Theoretically, it may be something else other than int
    printf("num of consumers=%d, demand=%d\nnum of suppliers=%d, supply unit=%d\n", cons_num, demand_unit, sup_num, supply_unit);
    if ((shmid = shmget(key[i], memseg_size, IPC_CREAT | IPC_EXCL | PERMS)) < 0) { // allocates shared memory returning SHMID - shared memory identifier
        handle_error("shmget", errno);
        status = EXIT_FAILURE;
        goto end;
    }
    i++;

    nsems = sup_num; // sup_num defines a number of semaphores
    if ((semid = semget(key[i], nsems, IPC_CREAT | IPC_EXCL | PERMS)) < 0) { // Create a new semaphore set of nsems semaphores
        handle_error("semget", errno);
        status = EXIT_FAILURE;
        goto end;
    }
    i++;

    if ((jobs_semid = semget(key[i], JOBS_NSEMS_NUM, IPC_CREAT | IPC_EXCL | PERMS)) < 0) {
        handle_error("semget", errno);
        status = EXIT_FAILURE;
        goto end;
    }
    i = 0;

    arg.val = 1; // set semaphore semnum's semval to 1, make it available
    for (i = 0; i < nsems; i++) {
        if (semctl(semid, i, SETVAL, arg) < 0) {
            handle_error("semctl", errno);
            status = EXIT_FAILURE;
            goto end;
        }
    }
    i = 0;

    if (semctl(jobs_semid, i, SETVAL, arg) < 0) {
        handle_error("semctl", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    if ((memseg = shmat(shmid, NULL, 0)) == (void *)-1) { //attaches the base address of shared memory.
        handle_error("shmat", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    consumer = (consumer_t *)memseg; // cast raw memory area to consumer_t *
    supplier = (supplier_t *)(consumer + cons_num); // cons_num is cast to larger type (consumer_t): (cons_num * sizeof(consumer_t))
    jobs = (int *)((supplier_t *)supplier + sup_num);

    for (i = 0; i < cons_num; i++)
        consumer[i].dem_unit = demand_unit;

    for (i = 0; i < sup_num; i++)
        supplier[i].sup_unit = supply_unit;
    *jobs = cons_num;

    for (pidx = i = 0; i < cons_num; i++, pidx++) {
        if ((pid_list[pidx] = fork()) < 0) {
            status = EXIT_FAILURE;
            goto end;
        }
        if (pid_list[pidx] == 0)
            consume(shmid, semid, jobs_semid, nsems, i, cons_num);
    }

    for (i = 0; i < sup_num; i++, pidx++) {
        if ((pid_list[pidx] = fork()) < 0) {
            status = EXIT_FAILURE;
            goto end;
        }
        if (pid_list[pidx] == 0)
            supply(shmid, semid, jobs_semid, cons_num, i, sup_num, supply_unit);
    }

end: // deallocation section
    if (pid_list) {
        for (i = 0; i < proc_num; i++) {
            if (pid_list[i] > 0) {
                if (status == EXIT_FAILURE)
                    (void)kill(pid_list[i], SIGKILL);
                if (waitpid(pid_list[i], &wret, 0) < 0) {
                    handle_error("waitpid", errno);
                    status = EXIT_FAILURE;
                    continue;
                }
                if (WIFEXITED(wret)) 
                    printf("%d exited, wret = %d\n", pid_list[i], WEXITSTATUS(wret));
                else if (WIFSIGNALED(wret)) 
                    printf("%d killed by signal %d\n", pid_list[i], WTERMSIG(wret));
                pid_list[i] = -1;
            }
        }
        free(pid_list);
    }

    if (status != EXIT_FAILURE)
        printf("Every consumer has been satisfied\n");

    if (memseg && shmdt(memseg) < 0) {
        handle_error("shmdt", errno);
        status = EXIT_FAILURE;
    }

    if (shmid != -1 && shmctl(shmid, IPC_RMID, NULL) < 0) {
        handle_error("shmctl", errno);
        status = EXIT_FAILURE;
    }

    if (semid != -1 && semctl(semid, 0, IPC_RMID, NULL) < 0) { // remove the semaphore set, semnum is ignored, so 0
        handle_error("semctl", errno);
        status = EXIT_FAILURE;
    }

    if (jobs_semid != -1 && semctl(jobs_semid, 0, IPC_RMID, NULL) < 0) { // remove the semaphore set, semnum is ignored, so 0
        handle_error("semctl", errno);
        status = EXIT_FAILURE;
    }

    status = status != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return status;
print:
    print_usage(prog_name);
    return status;
}

void consume(int shmid, int semid, int jobs_semid, const int sup_num, const int cons_idx, const int cons_num)
{
    void *memseg = NULL;
    consumer_t *consumer = NULL;
    struct sembuf lock, unlock, jobs_lock, jobs_unlock;
    int sup_idx, status = EXIT_SUCCESS;
    const int semops = 1, jobs_idx = 0;

    if ((memseg = shmat(shmid, NULL, 0)) == (void *)-1) { //attaches the base address of shared memory.
        handle_error("shmat", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    consumer = (consumer_t *)memseg + cons_idx;

    jobs_lock.sem_num = jobs_unlock.sem_num = jobs_idx;
    lock.sem_op = jobs_lock.sem_op = CONSUME;
    unlock.sem_op = jobs_unlock.sem_op = RETURN;
    lock.sem_flg = unlock.sem_flg = jobs_lock.sem_flg = jobs_unlock.sem_flg = SEM_UNDO;

    (void)srand(time(NULL) ^ getpid()); // choose a unique seeding value to generate random sequences

    while (true) {
        supplier_t *supplier = NULL;
        int *jobs = NULL;
        int take = 0;
        sup_idx = rand() % sup_num;
        lock.sem_num = unlock.sem_num = (unsigned short)sup_idx;
        /* semval is either 1 (AVAILABLE/RELEASED STATE) or 0 (BUSY/OCCUPIED STATE), the latter of which has the process block on semop() */
        if (semop(semid, &lock, semops) < 0) {
            handle_error("semop", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        /* CRITICAL SECTION START: the process acquires the lock/semaphore. The process is blocked on semop() when trying to acquire the same lock/semaphore */
        supplier = (supplier_t *)((consumer_t *)memseg + cons_num) + sup_idx;
        printf("Consumer%d: demand=%d, supplier%d's stock=%d\n", cons_idx, consumer->dem_unit, sup_idx, supplier->sup_unit);
        if (supplier->sup_unit <= 0) {
            printf("Consumer%d awaits because supplier%d is empty\n", cons_idx, sup_idx);
            if (semop(semid, &unlock, semops) < 0) {
                handle_error("semop", errno);
                status = EXIT_FAILURE;
                goto end;
            }
            sleep(3);
            continue;
        }        
        /* if demand is less than stock, take exactly what you need, other wise consume the whole unit */
        take = consumer->dem_unit < supplier->sup_unit ? consumer->dem_unit : supplier->sup_unit;
        consumer->dem_unit -= take;
        supplier->sup_unit -= take;
        if (consumer->dem_unit <= 0) {
            printf("Consumer%d is satisfied. Leaving...\n", cons_idx);
            /* CRITICAL SECTION END */
            if (semop(semid, &unlock, semops) < 0) {
                handle_error("semop", errno);
                status = EXIT_FAILURE;
                goto end;
            }
            /* CRITICAL SECTION FOR FLAG START */
            if (semop(jobs_semid, &jobs_lock, semops) < 0) {
                handle_error("semop", errno);
                status = EXIT_FAILURE;
                goto end;
            }
            jobs = (int *)((supplier_t *)((consumer_t *)memseg + cons_num) + sup_num);
            (*jobs)--;
            printf("Jobs left: %d\n", *jobs);
            /* CRITICAL SECTION FOR FLAG END */
            if (semop(jobs_semid, &jobs_unlock, semops) < 0) {
                handle_error("semop", errno);
                status = EXIT_FAILURE;
                goto end;
            }
            break;
        }
        /* CRITICAL SECTION END: The processes releases the lock/semaphore. The next process may be unblocked just in time to re-acquire the lock above */
        if (semop(semid, &unlock, semops) < 0) {
            handle_error("semop", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        sleep(2); // Take a smoke, drink some beer before looping through again
    }
end:
    if (memseg && shmdt(memseg) < 0) {
        handle_error("shmdt", errno);
        status = EXIT_FAILURE;
    }
    printf("Consumer%d is exiting. exit status = %d\n", cons_idx, status);
    exit(status);
}

void supply(int shmid, int semid, int jobs_semid, const int cons_num, const int sup_idx, const int sup_num, const int restock_amount)
{
    void *memseg = NULL;
    supplier_t *supplier = NULL;
    struct sembuf lock, unlock, jobs_lock, jobs_unlock;
    int status = EXIT_SUCCESS;
    const int semops = 1, jobs_idx = 0;

    if ((memseg = shmat(shmid, NULL, 0)) == (void *)-1) { //attaches the base address of shared memory.
        handle_error("shmat", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    supplier = (supplier_t *)((consumer_t *)memseg + cons_num) + sup_idx;

    jobs_lock.sem_num = jobs_unlock.sem_num = jobs_idx;
    lock.sem_num = unlock.sem_num = (unsigned short)sup_idx;
    lock.sem_op = jobs_lock.sem_op = CONSUME;
    unlock.sem_op = jobs_unlock.sem_op = RETURN;
    lock.sem_flg = jobs_lock.sem_flg = unlock.sem_flg = jobs_unlock.sem_flg = SEM_UNDO;

    while (true) {
        int *jobs = NULL;
        /* semval is either 1 (AVAILABLE STATE) or 0 (TAKEN STATE), the latter of which has the process block on semop() */
        if (semop(semid, &lock, semops) < 0) {
            handle_error("semop", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        supplier->sup_unit += restock_amount;
        printf("Restocking the supplies for supplier%d: stock=%d\n", sup_idx, supplier->sup_unit);
        /* CRITICAL SECTION END: the supplier released the resource (AVAILABLE STATE) */
        if (semop(semid, &unlock, semops) < 0) {
            handle_error("semop", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        /* CRITICAL SECTION FOR FLAG END */
        if (semop(jobs_semid, &jobs_lock, semops) < 0) {
            handle_error("semop", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        /* CRITICAL SECTION FOR FLAG START */
        jobs = (int *)((supplier_t *)((consumer_t *)memseg + cons_num) + sup_num);
        if (*jobs <= 0) {
            printf("Supplier%d is out of customers.\n", sup_idx);
            /* CRITICAL SECTION FOR FLAG END */
            if (semop(jobs_semid, &jobs_unlock, semops) < 0) {
                handle_error("semop", errno);
                status = EXIT_FAILURE;
                goto end;
            }
            break;
        }
        /* CRITICAL SECTION FOR FLAG END */
        if (semop(jobs_semid, &jobs_unlock, semops) < 0) {
            handle_error("semop", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        sleep(1);
    }
end:
    if (memseg && shmdt(memseg) < 0) {
        handle_error("shmdt", errno);
        status = EXIT_FAILURE;
    }
    printf("Supplier%d is leaving. exit status = %d\n", sup_idx, status);
    exit(status);
}

void print_usage(const char *prog_name)
{
    printf("Usage:\n");
    printf("1) %s -%c\n", prog_name, HELP_OPT);
    printf("2) %s -%c 4 -%c 4\n", prog_name, CONSUMER_OPT, SUPPLIER_OPT);
    printf("3) %s -%c 4 -%c 4 -%c 5000 -%c 500\n", prog_name, CONSUMER_OPT, SUPPLIER_OPT, DEMAND_OPT, SUPPLY_OPT);
}

bool options_bundled(int opt, const char *optarg)
{
    bool bundled = false;

    if (opt <= 0 || !optarg)
        return false;
    if (*optarg == '-') {
        handle_error_msg("error: bundled options, i.e \"-%c%c\" are not allowed\n", opt, *optarg);
        bundled = true;
    }

    return bundled;
}