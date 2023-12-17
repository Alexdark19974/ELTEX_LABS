#include "address_book.h"

void clear_stdin_buffer(void)
{
    while (getchar() != '\n') ;
}

void free_all(LedgerP *head)
{
    if (!head || !*head)
        return;

    for (LedgerP p = *head, q = NULL; p;  p = q) {
        q = p->next;
        free(p); p = NULL;
    }
}

