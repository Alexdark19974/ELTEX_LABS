#include "address_book.h"

void add_customer(LedgerP *head)
{
    LedgerP p = NULL;

    p = calloc(1, sizeof(Ledger));
    if (!p) {
        fprintf(stderr, "[%s:%s:%d] Failed to allocate memory (%s)\n", __FILE__, /*__FUNCTION__, supress a compiler warning */ __func__, __LINE__, strerror(errno));
        return;
    }

    printf("1) Name: ");
    if (!fgets(p->name, sizeof(p->name), stdin)) {
        fprintf(stderr, "error: fgets failed (%s)\n", strerror(errno));
        return;
    }

    if (p->name[0])
        p->name[strlen(p->name) - 1] = '\0'; // remove '\n'

    printf("2) Middle name: ");
    if (!fgets(p->middle_name, sizeof(p->middle_name), stdin)) {
        fprintf(stderr, "error: fgets failed (%s)\n", strerror(errno));
        return;
    }
    if (p->middle_name[0])
        p->middle_name[strlen(p->middle_name) - 1] = '\0';

    printf("3) Last Name: ");
    if (!fgets(p->last_name, sizeof(p->last_name), stdin)) {
        fprintf(stderr, "error: fgets failed (%s)\n", strerror(errno));
        return;
    }
    if (p->last_name[0])
        p->last_name[strlen(p->last_name) - 1] = '\0';

    printf("4) Phone number: ");
    if (!fgets(p->number, sizeof(p->number), stdin)) {
        fprintf(stderr, "error: fgets failed (%s)\n", strerror(errno));
        return;
    }
    if (p->number[0])
        p->number[strlen(p->number) - 1] = '\0';
    p->next = NULL;

    printf("Customer:\nname:%s\nmiddle name:%s\nlast name:%s\nphone number:%s\n", p->name, p->middle_name, p->last_name, p->number);

    if (!*head) {
        *head = p;
    } else {
        p->next = *head;
        *head = p;
    }
}

void remove_customer(LedgerP *head)
{
    char buf[BUFSIZE] = {0};

    if (!head || !*head) {
        printf("warning: nobody to delete.\n");
        return;
    }

    if (!fgets(buf, sizeof(buf), stdin)) {
        fprintf(stderr, "error: fgets failed (%s)", strerror(errno));
        return;
    }

    if (isspace(buf[0])) {
        printf("warning: no input. Remove routine terminated.\n");
        return;
    }

    if (buf[0]) {
        buf[strlen(buf) - 1] = '\0';
    }

    for (LedgerP p = *head, q = NULL; p; p = p->next) {
        if (!strcmp((*head)->name, buf)) { // free at the beginning
            p = (*head)->next;
            free(*head);
            *head = NULL, *head = p;
            printf("Found and deleted %s\nNew head:%p\nhead->name = %s\nhead->next:%p\n", buf, (void *)*head, (*head)->name, (void *) (*head)->next);
            break;
        } else if (p->next) { // free in the middle or at the end
            if (!strcmp(p->next->name, buf)) {
                q = p->next;
                printf("Found and deleted %s %s\n", buf, q->next ? "in the middle" : "at the end");
                p->next = q->next;
                free(q); q = NULL;
                break;
            }
        }
    }
}

void find_customer(LedgerP *head)
{
    char buf[BUFSIZE] = {0};

    if (!fgets(buf, sizeof(buf), stdin)) {
        fprintf(stderr, "error: fgets failed (%s)\n", strerror(errno));
        return;
    }
    if (isspace(buf[0])) {
        printf("warning: no input. Customer routine terminated.\n");
        return;
    }

    if (buf[0]) {
        buf[strlen(buf) - 1] = '\0';
    }

    for (LedgerP p = *head; p; p = p->next) {
        if (!strcmp(p->name, buf)) {
            printf("Found customer:\nname:%s\nmiddle name:%s\nlast name:%s\nphone number:%s\n", p->name, p->middle_name, p->last_name, p->number);
            break;
        }
    }
}

void display_customer(LedgerP *head)
{
    LedgerP p = *head;
    unsigned int i = 0;

    while (p) {
        puts("----------------------------");
        printf("Customer %u:\nname:%s\nmiddle name:%s\nlast name:%sphone number:%s\nnext = %p\n", ++i, p->name, p->middle_name, p->last_name, p->number, (void *) p->next);
        puts("----------------------------");
        p = p->next;
    }
}

void display_options(void)
{
    printf("Press 1 to add a new customer\n"
           "Press 2 to display customers\n"
           "Press 3 to find a customer\n"
           "Press 4 to display options\n"
           "Press 5 to remove a customer\n"
           "Press 6 to exit\n");
}

