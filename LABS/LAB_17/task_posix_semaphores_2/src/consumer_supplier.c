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
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>
#define DEFAULT_FILE_NAME "/cs"
#define CONSUMER_OPT 'c'
#define SUPPLIER_OPT 's'
#define DEMAND_OPT 'D'
#define SUPPLY_OPT 'S'
#define HELP_OPT 'h'
#define BUF_SIZ 256
#define PERMS 0666 // -rw-rw-rw
#define SUPPLY_UNIT 500
#define DEMAND_UNIT 5000
#define SEM_VAL 1
#define JOBS_SUFFIX "jobs"
#define SHM_SUFFIX "shm"
#define SEM_SUFFIX "sem"
#define SUFFIX_LEN 3
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

typedef struct supplier_s {
    int sup_unit;
} supplier_t;

typedef struct consumer_s {
    int dem_unit;
} consumer_t;

void print_usage(const char *prog_name);
bool options_bundled(int opt, const char *optarg);
int consumer_supplier(int argc, char **argv);
void consume(void *memseg, size_t memseg_size, sem_t **smphr, sem_t *jobs_smphr, const int sup_num, const int cons_idx, const int cons_num);
void supply(void *memseg, size_t memseg_size, sem_t *smphr, sem_t *jobs_smphr, const int cons_num, const int sup_idx, const int sup_num, const int restock_amount);

int main(int argc, char **argv)
{
    return consumer_supplier(argc, argv);
}

int consumer_supplier(int argc, char **argv)
{
    const char *prog_name = NULL;
    char **sem_name = NULL, shm_name[BUF_SIZ] = {0}, sem_jobs_name[BUF_SIZ] = {0};
    void *memseg = NULL;
    pid_t *pid_list = NULL;
    int *jobs = NULL;
    consumer_t *consumer = NULL;
    supplier_t *supplier = NULL;
    sem_t **smphr = NULL, *jobs_smphr = NULL;
    size_t memseg_size = 0, cons_size = 0, sup_size = 0;
    long num = 0;
    int opt = 0, wret = 0, status = EXIT_SUCCESS;
    int shmfd = -1;
    int cons_num = 0, sup_num = 0, proc_num = 0, supply_unit = 0, demand_unit = 0;
    int i, pidx;

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

    proc_num = cons_num + sup_num;
    if (!(pid_list = calloc((size_t)proc_num, sizeof(pid_t)))) {
        handle_error("calloc", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    snprintf(shm_name, sizeof(shm_name), "%s_%s", DEFAULT_FILE_NAME, SHM_SUFFIX);
    printf("shared memory name = %s\n", shm_name);

    cons_size = cons_num * sizeof(consumer_t); // size of consumers
    sup_size = sup_num * sizeof(supplier_t); // size of suppliers
    memseg_size = cons_size + sup_size + sizeof(int); // calculate size of jobs_num. Theoretically, it may be something else other than int

    printf("num of consumers=%d, demand=%d\nnum of suppliers=%d, supply unit=%d\n", cons_num, demand_unit, sup_num, supply_unit);

    /* SHARED MEMORY ALLOCATION AND INITLIATION START */
    /* create a handle (i.e. a file descriptor) to manage a shared memory object */
    if ((shmfd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, PERMS)) < 0) {
        handle_error("shm_open", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    /* ftruncate sets the shared memory object's ("file") size to nbytes */
    if (ftruncate(shmfd, memseg_size) < 0) {
        handle_error("ftruncate", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    if ((memseg = mmap(NULL, memseg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0)) == (void *)-1) {
        handle_error("mmap", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    (void)close(shmfd);
    shmfd = -1;

    consumer = (consumer_t *)memseg; // cast raw memory area to consumer_t *
    supplier = (supplier_t *)(consumer + cons_num); // add by elements of sizeof(cons_num) type, not bytes
    jobs = (int *)((supplier_t *)supplier + sup_num);

    for (i = 0; i < cons_num; i++)
        consumer[i].dem_unit = demand_unit;

    for (i = 0; i < sup_num; i++)
        supplier[i].sup_unit = supply_unit;
    *jobs = cons_num;
    /* SHARED MEMORY ALLOCATION AND INITLIATION END */

    /* SEMAPHORE MEMORY ALLOCATION AND INITLIATION START */
    if (!(sem_name = calloc(sup_num, sizeof(char *)))) { /* allocate sem_name pointers of char * type */
        handle_error("calloc", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    for (i = 0; i < sup_num; i++) {
        if (!(sem_name[i] = calloc(1, BUF_SIZ))) { /* allocate sem_name[i] array of char typea nd BUF_SIZ size long */
            handle_error("calloc", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        snprintf(sem_name[i], BUF_SIZ, "%s_%s_%d", DEFAULT_FILE_NAME, SEM_SUFFIX, i);
        printf("semaphore%d's name=%s\n", i, sem_name[i]);
    }

    if (!(smphr = calloc(sup_num, sizeof(sem_t *)))) { // allocate sup_num pointers of sem_t * type
        handle_error("calloc", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    for (i = 0; i < sup_num; i++) {
        /*sem_open returns an address a of new semaphore, which is a handle just like shmfd. Do NOT free the return value. */
        if ((smphr[i] = sem_open(sem_name[i], O_CREAT | O_EXCL | O_RDWR, PERMS, SEM_VAL)) == SEM_FAILED) {
            handle_error("sem_open", errno);
            status = EXIT_FAILURE;
            goto end;
        }
    }
    /* SEMAPHORE MEMORY ALLOCATION AND INITLIATION END */

    snprintf(sem_jobs_name, sizeof(sem_jobs_name), "%s_%s", DEFAULT_FILE_NAME, JOBS_SUFFIX);
    printf("jobs semaphore's name=%s\n", sem_jobs_name);

    if ((jobs_smphr = sem_open(sem_jobs_name, O_CREAT | O_EXCL | O_RDWR, PERMS, SEM_VAL)) == SEM_FAILED) {
        handle_error("sem_open", errno);
        status = EXIT_FAILURE;
        goto end;
    }

    for (pidx = i = 0; i < cons_num; i++, pidx++) {
        if ((pid_list[pidx] = fork()) < 0) {
            status = EXIT_FAILURE;
            goto end;
        }
        if (pid_list[pidx] == 0)
            consume(memseg, memseg_size, smphr, jobs_smphr, sup_num, i, cons_num);
    }

    for (i = 0; i < sup_num; i++, pidx++) {
        if ((pid_list[pidx] = fork()) < 0) {
            status = EXIT_FAILURE;
            goto end;
        }
        if (pid_list[pidx] == 0)
            supply(memseg, memseg_size, smphr[i], jobs_smphr, cons_num, i, sup_num, supply_unit);
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

    if (memseg && munmap(memseg, memseg_size) < 0) {
        handle_error("munmap", errno);
        status = EXIT_FAILURE;
    }

    if (shmfd != -1)
        (void)close(shmfd);

    if (*shm_name)
        (void)shm_unlink(shm_name);

    if (sem_name) {
        for (i = 0; i < sup_num; i++) {
            if (sem_name[i]) {
                (void)sem_unlink(sem_name[i]);
                free(sem_name[i]);
            }
        }
        if (sem_name)
            free(sem_name); 
    }

    if (smphr) {
        for (i = 0; i < sup_num; i++) {
            if (smphr[i] && smphr[i] != SEM_FAILED)
                (void)sem_close(smphr[i]);
        }
        if (smphr)
            free(smphr);
    }

    if (jobs_smphr && jobs_smphr != SEM_FAILED)
        (void)sem_close(jobs_smphr);

    if (*sem_jobs_name)
        (void)sem_unlink(sem_jobs_name);

    status = status != 0 ? EXIT_FAILURE : EXIT_SUCCESS;
    return status;
print:
    print_usage(prog_name);
    return status;
}

void consume(void *memseg, size_t memseg_size, sem_t **smphr, sem_t *jobs_smphr, const int sup_num, const int cons_idx, const int cons_num)
{
    consumer_t *consumer = NULL;
    int sup_idx, i, sem_num = 0, status = EXIT_SUCCESS;

    consumer = (consumer_t *)memseg + cons_idx;

    (void)srand(time(NULL) ^ getpid()); // choose a unique seeding value to generate random sequences

    while (true) {
        supplier_t *supplier = NULL;
        int *jobs = NULL;
        int take = 0;
        sup_idx = rand() % sup_num;

        if (sem_wait(smphr[sup_idx]) < 0) {
            handle_error("sem_wait", errno);
            status = EXIT_FAILURE;
            goto end;
        }

        /* CRITICAL SECTION START: the process acquires the lock/semaphore. The process is blocked on semop() when trying to acquire the same lock/semaphore */
        supplier = (supplier_t *)((consumer_t *)memseg + cons_num) + sup_idx;
        printf("Consumer%d: demand=%d, supplier%d's stock=%d\n", cons_idx, consumer->dem_unit, sup_idx, supplier->sup_unit);
        if (supplier->sup_unit <= 0) {
            printf("Consumer%d awaits because supplier%d is empty\n", cons_idx, sup_idx);
            if (sem_post(smphr[sup_idx]) < 0) {
                handle_error("sem_post", errno);
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
            if (sem_post(smphr[sup_idx]) < 0) {
                handle_error("sem_post", errno);
                status = EXIT_FAILURE;
                goto end;
            }
            /* CRITICAL SECTION FOR FLAG START */
            if (sem_wait(jobs_smphr) < 0) {
                handle_error("sem_wait", errno);
                status = EXIT_FAILURE;
                goto end;
            }
            jobs = (int *)((supplier_t *)((consumer_t *)memseg + cons_num) + sup_num);
            (*jobs)--;
            printf("Jobs left: %d\n", *jobs);
            /* CRITICAL SECTION FOR FLAG END */
            if (sem_post(jobs_smphr) < 0) {
                handle_error("sem_post", errno);
                status = EXIT_FAILURE;
                goto end;
            }
            break;
        }

        /* CRITICAL SECTION END: AVAILABLE STATE */
        if (sem_post(smphr[sup_idx]) < 0) {
            handle_error("sem_post", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        sleep(2); // Take a smoke, drink some beer before looping through again
    }
end:
    if (memseg && munmap(memseg, memseg_size) < 0) {
        handle_error("munmap", errno);
        status = EXIT_FAILURE;
    }

    for (i = 0; i < sup_num; i++)
        (void)sem_close(smphr[i]);

    (void)sem_close(jobs_smphr);

    printf("Consumer%d is exiting. exit status = %d\n", cons_idx, status);
    exit(status);
}

void supply(void *memseg, size_t memseg_size, sem_t *smphr, sem_t *jobs_smphr, const int cons_num, const int sup_idx, const int sup_num, const int restock_amount)
{
    supplier_t *supplier = NULL;
    int status = EXIT_SUCCESS;

    supplier = (supplier_t *)((consumer_t *)memseg + cons_num) + sup_idx;

    while (true) {
        int *jobs = NULL;
        if (sem_wait(smphr) < 0) {
            handle_error("sem_wait", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        supplier->sup_unit += restock_amount;
        printf("Restocking the supplies for supplier%d: stock=%d\n", sup_idx, supplier->sup_unit);
        /* CRITICAL SECTION END: the supplier released the resource (AVAILABLE STATE) */
        if (sem_post(smphr) < 0) {
            handle_error("sem_post", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        /* CRITICAL SECTION FOR FLAG END */
        if (sem_wait(jobs_smphr) < 0) {
            handle_error("sem_wait", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        /* CRITICAL SECTION FOR FLAG START */
        jobs = (int *)((supplier_t *)((consumer_t *)memseg + cons_num) + sup_num);
        if (*jobs <= 0) {
            printf("Supplier%d is out of customers.\n", sup_idx);
            /* CRITICAL SECTION FOR FLAG END */
            if (sem_post(jobs_smphr) < 0) {
                handle_error("sem_post", errno);
                status = EXIT_FAILURE;
                goto end;
            }
            break;
        }
        /* CRITICAL SECTION FOR FLAG END */
        if (sem_post(jobs_smphr) < 0) {
            handle_error("sem_post", errno);
            status = EXIT_FAILURE;
            goto end;
        }
        sleep(1);
    }
end:
    if (memseg && munmap(memseg, memseg_size) < 0) {
        handle_error("munmap", errno);
        status = EXIT_FAILURE;
    }

    (void)sem_close(smphr);

    (void)sem_close(jobs_smphr);

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