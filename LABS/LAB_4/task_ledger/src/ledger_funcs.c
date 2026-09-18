#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <ctype.h>
#include "address_book.h"

static int process_param(void *data, size_t size, TYPE data_type);
extern Ledger ledger;

int add(void)
{
    Priv newPriv = {0};
    int i;

    printf("1) Name: ");
    if (ERR == process_param((void *)newPriv.name, sizeof(newPriv.name), STRING)) {
        goto fail;
    }

    printf("2) Middle name: ");
    if (ERR == process_param((void *)newPriv.middle_name, sizeof(newPriv.middle_name), STRING)) {
        goto fail;
    }

    printf("3) Last Name: ");
    if (ERR == process_param((void *)newPriv.last_name, sizeof(newPriv.last_name), STRING)) {
        goto fail;
    }

    printf("4) Phone number: ");
    if (ERR == process_param((void *)newPriv.phone_number, sizeof(newPriv.phone_number), STRING)) {
        goto fail;
    }

    printf("5) Occupation: ");
    if (ERR == process_param((void *)newPriv.occupation, sizeof(newPriv.occupation), STRING)) {
        goto fail;
    }

    printf("6) Age: ");
    if (ERR == process_param((void *)&newPriv.age, sizeof(newPriv.age), INT)) {
        goto fail;
    }

    printf(
        "Customer:\n"
        " 1) Name:%s;\n"
        " 2) Middle name:%s;\n"
        " 3) Last name:%s;\n"
        " 4) Phone number:%s\n"
        " 5) Occupation:%s\n"
        " 6) Age: %d\n",
        newPriv.name,
        newPriv.middle_name,
        newPriv.last_name,
        newPriv.phone_number,
        newPriv.occupation,
        newPriv.age
    );

    for (i = 0; i < MAX_PRIV; i++) {
        if (*ledger.priv[i].name == '\0') {
            if (memcpy((void *)&ledger.priv[i], (const void *)&newPriv, sizeof(ledger.priv[i])) == NULL) {
                 fprintf(stderr, "memcpy failed; cause: %s\n", strerror(errno));
                 goto fail;
            }

            i = 0;
            break;
        }
    }

    if (i >= MAX_PRIV)
        fprintf(stderr, "failed to add another customer: out of space (i = %d, MAX_PRIV = %d\n", i, MAX_PRIV);

    printf("Added the new customer!\n");

    return OK;
fail:
    return ERR;
}

int remove_(void)
{
    char buf[BUFSIZE] = {0};
    int c = 0, i;

    printf("1) Name: ");
    if (ERR == process_param((void *)buf, BUFSIZE, STRING)) {
        goto fail;
    }

    for (i = 0; i < MAX_PRIV; i++) {
        if (!strcmp(buf, ledger.priv[i].name)) {
            printf(
                "Found customer:\n"
                " 1) Name:%s;\n"
                " 2) Middle name:%s;\n"
                " 3) Last name:%s;\n"
                " 4) Phone number:%s\n"
                " 5) Occupation:%s\n"
                " 6) Age: %d\n",
                ledger.priv[i].name,
                ledger.priv[i].middle_name,
                ledger.priv[i].last_name,
                ledger.priv[i].phone_number,
                ledger.priv[i].occupation,
                ledger.priv[i].age
            );

            DECIDE("Delete? y/n\n", c);

            if (c == EOF) {
                fprintf(stderr, "Caught EOF (%s)\n", strerror(errno));
                break;
            }

            clear_stdin_buffer();

            if ((c = tolower(c)) == 'n' || c != 'y') {
                printf("Skipping %s\n", ledger.priv[i].name);
                continue;
            }

            memset((void *)&ledger.priv[i], 0, sizeof(ledger.priv[i]));

            i = 0;
            break;
        }
    }

   if (i >= MAX_PRIV)
        fprintf(stderr, "failed to remove the customer '%s' (Not in the ledger)\n", buf);

    return OK;
fail:

    return ERR;
}

int find(void)
{
    char buf[BUFSIZE] = {0};
    int c = 0, i;

    printf("Enter name: ");
    if (ERR == process_param((void *)buf, BUFSIZE, STRING)) {
        goto fail;
    }

    for (i = 0; i < MAX_PRIV; i++) {
        if (!strcmp(ledger.priv[i].name, buf)) {
            printf(
                "Found customer:\n"
                " 1) Name:%s;\n"
                " 2) Middle name:%s;\n"
                " 3) Last name:%s;\n"
                " 4) Phone number:%s\n"
                " 5) Occupation:%s\n"
                " 6) Age: %d\n",
                ledger.priv[i].name,
                ledger.priv[i].middle_name,
                ledger.priv[i].last_name,
                ledger.priv[i].phone_number,
                ledger.priv[i].occupation,
                ledger.priv[i].age
            );

            DECIDE("Keep searching? y/n\n", c);

            if (c == EOF) {
                fprintf(stderr, "Caught EOF; cause: %s\n", strerror(errno));
                break;
            }

            clear_stdin_buffer();

            if ((c = tolower(c)) == 'y' || c != 'n') {
                printf("Skipping '%s'\n", ledger.priv[i].name);
                continue;
            }

            i = 0;
            break;
        }
    }

    if (i >= MAX_PRIV)
        fprintf(stderr, "failed to find customer '%s' (Not in the ledger)\n", buf);

    return OK;
fail:
    return ERR;
}

void display(void)
{
    unsigned int i, j;

    for (i = 0, j = 0; i < MAX_PRIV; i++) {
        if (*ledger.priv[i].name == '\0')
            break;
        puts("----------------------------");
        printf(
            "Customer %u:\n"
            " 1) Name:%s\n"
            " 2) Middle name:%s\n"
            " 3) Last name:%s\n"
            " 4) Phone number:%s\n"
            " 5) Occupation:%s\n"
            " 6) Age:%d\n",
            ++j,
            ledger.priv[i].name,
            ledger.priv[i].middle_name,
            ledger.priv[i].last_name,
            ledger.priv[i].phone_number,
            ledger.priv[i].occupation,
            ledger.priv[i].age
        );
        puts("----------------------------");
    }
}

static int process_param(void *data, size_t size, TYPE data_type)
{
    char *subStr = NULL;
    int ret = 0;

    switch (data_type) {

        case STRING:
            if (!fgets((char *)data, (int) size, stdin)) {
                fprintf(stderr, "error: fgets failed (%s)\n", strerror(errno));
                return ERR;
            }

            if ((subStr = strchr((char *)data, '\n')) == NULL) {
                fprintf(stderr, "error: substr failed (%s)", strerror(errno));
                goto fail;
            }

            *subStr = '\0';

            if ((char *)data == subStr) {
                printf("warning: no input. Routine terminated.\n");
                goto fail;
            }

            break;
        case INT:
try_again:
            ret = scanf("%d", (int *)data);

            if (ret <= 0) {
                fprintf(stderr, "error: scanf failed (%s)", strerror(errno));
                goto fail;
            }

            clear_stdin_buffer();

            if (*(int *)data <= 0) {
                printf("age cannot be 0 or less than.\nTry again.\n");
                goto try_again;
            }

            break;
        default:
            printf("Uknown data type = %d\n", data_type);
            break;
    }
    return OK;
fail:
    return ERR;
}

void clear_stdin_buffer(void)
{
    while (getchar() != '\n') ;
}
