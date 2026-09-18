#include <stdio.h>
#include <malloc.h>
#include "address_book.h"
#include "../include/address_book_internals.h"

static inline void free_all(LedgerP *head)
{
    if (!head || !*head)
        return;

    for (NodeP p = (*head)->head, q = NULL; p;  p = q) {
        q = p->next;
        free(p); p = NULL;
    }
}

void *constructor(void)
{
    void *obj = (void *) malloc(sizeof(Ledger));

    return obj;
}

void destructor(void **obj)
{
    free_all((LedgerP *)obj);
    (*(LedgerP *)obj)->add = NULL;
    (*(LedgerP *)obj)->remove_by = NULL;
    (*(LedgerP *)obj)->find_by = NULL;
    (*(LedgerP *)obj)->load_from = NULL;
    (*(LedgerP *)obj)->display = NULL;
    free(*(LedgerP *)obj);
    *(LedgerP *)obj = NULL;
}

void init_obj(void **obj)
{
    (*(LedgerP *)obj)->add = &add;
    (*(LedgerP *)obj)->remove_by = &remove_by;
    (*(LedgerP *)obj)->find_by = &find_by;
    (*(LedgerP *)obj)->load_from = &load_from;
    (*(LedgerP *)obj)->display = &display;
    (*(LedgerP *)obj)->head = NULL;
}
