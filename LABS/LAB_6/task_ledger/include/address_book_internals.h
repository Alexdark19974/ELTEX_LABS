#ifndef ADRESS_BOOK_INTERNALS_H
#define ADRESS_BOOK_INTERNALS_H
/* Private Data */
#include "address_book.h"
#include "cleanup.h"
#include <errno.h>
#include <string.h>
extern void *constructor(void);
extern int add(void **obj, PrivP entry, int pos);
extern int remove_by(void **obj, int opt);
extern int find_by(void **obj, int opt);
extern int load_from(void **obj, const char *filename);
extern void display(void **obj);
extern void destructor(void **obj);
extern void init_obj(void **obj);
extern int address_book(void);
extern void *constructor(void);
extern void init_obj(void **obj);
#define NAMESIZ 64
#define OCCUPSIZ 32
#define DECIDE(message, c) do { printf(message); c = getc(stdin); } while(0)

#define PRINT_ERR(str) do { \
                            fprintf(stderr, "[%s:%d] %s\n", __func__, __LINE__, str); \
                            if (errno) \
                                fprintf(stderr, "[%s:%d] cause: %s\n", __func__, __LINE__, strerror(errno)); \
                         } while(0)

struct priv {
    char name[NAMESIZ];
    char mname[NAMESIZ];
    char lname[NAMESIZ];
    char number[NAMESIZ];
    char occup[OCCUPSIZ];
    int age;
};

struct node {
    Priv priv;
    NodeP next;
};

struct ledger {
    int (*add)(void **obj, PrivP entry, int pos);
    int (*remove_by)(void **obj, int opt);
    int (*find_by)(void **obj, int opt);
    int (*load_from)(void **obj, const char *filename);
    void (*display)(void **obj);
    NodeP head;
    int entry_total;
};

#endif /* ADRESS_BOOK_INTERNALS_H */
