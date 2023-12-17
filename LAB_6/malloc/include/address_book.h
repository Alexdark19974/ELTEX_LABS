#ifndef ADDRESS_BOOK_H
#define ADDRESS_BOOK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#ifdef DMALLOC
#include "dmalloc.h"
#endif

#define NAME_SIZE 25
#define MIDDLE_NAME_SIZE 25
#define LAST_NAME_SIZE 25
#define NUMBER_SIZE 25
#define BUFSIZE 128

typedef struct Ledger Ledger, *LedgerP;
enum { ADD_CUSTOMER = 1, DISPLAY_CUSTOMERS, FIND_CUSTOMER, DISPLAY_OPTIONS, REMOVE_CUSTOMER, EXIT };
struct Ledger
{
    char name[BUFSIZE];
    char middle_name[BUFSIZE];
    char last_name[BUFSIZE];
    char number[BUFSIZE];
    LedgerP next;
};

void address_book (void);
void add_customer(LedgerP *);
void remove_customer(LedgerP *);
void find_customer(LedgerP *);
void display_customer(LedgerP *);
void display_options(void);
void list_options(void);
void clear_stdin_buffer(void);
void free_all(LedgerP *);

#endif // ADDRESS_BOOK_H

