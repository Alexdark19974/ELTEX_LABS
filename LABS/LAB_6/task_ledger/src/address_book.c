#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "address_book.h"

unsigned char debug = 0;

#include "../include/address_book_internals.h"
#include "../include/debug.h"
#include "../include/cleanup.h"

static void display_options(void);

int address_book(void)
{
    LedgerP ledger = NULL;
    // size_t len = 0;
    int c = 0, status = OK;
    char filename[NAMESIZ * 2] = {0};

#ifdef DEBUG
    debug = 1;
    DPRINT("Debug Mode");
#endif
    /* Allocate memory for ledger OOP-style */
    if (NULL == (ledger = constructor())) {
        PRINT_ERR("ERROR: constructor() failed!");
        return EXIT_FAILURE;
    }

    /* Initialize funcs OOP-style */
    init_obj((void **)&ledger);

    puts("-------------the address book--------------");
    puts("Press 6 to list commands");
    printf("Enter the command: ");

    while ((c = getchar()) != EOF) {
        if (!isspace(c))
            clear_stdin_buffer();
        switch (c - '0') {
            case ADD:
                DECIDE("Insert :\n1) at the beginning;\n2) in the middle;\n3) at the end;\n (default action - 1)", c);
                if (!isspace(c))
                    clear_stdin_buffer();
                c -= '0';
                if (c <= START || c >= END_END) {
                    printf("error: unknown option\n");
                    break;
                }
                status = ledger->add((void **)&ledger, NULL, c);
                break;
            case REMOVE:
                DECIDE("Remove by:\n1) entry (full info);\n2) name;\n3) middle name;\n4)last name;\n5) phone number;\n6) occupation;\n7) age;\n", c);
                if (!isspace(c))
                    clear_stdin_buffer();
                c -= '0';
                if (c <= BY_START || c >= BY_END) {
                    printf("error: unknown option\n");
                    break;
                }
                status = ledger->remove_by((void **)&ledger, c);
                break;
            case FIND:
                DECIDE("Find by:\n1) entry (full info);\n2) name;\n3) middle name;\n4) last name;\n5) phone number;\n6) occupation;\n7) age;\n", c);
                if (!isspace(c))
                    clear_stdin_buffer();
                c -= '0';
                if (c <= BY_START || c >= BY_END) {
                    printf("error: unknown option\n");
                    break;
                }
                status = ledger->find_by((void **)&ledger, c);
                break;
            case LOAD_FILE:
                printf("Enter the filename, including its path: ");
                if (!fgets(filename, sizeof(filename), stdin)) {
                    PRINT_ERR("ERROR: fgets() failed!");
                    status = ERR;
                } else {
                    if (!*filename || isspace(*filename)) {
                        printf("error: filename is empty!\n");
                        break;
                    }
                    filename[strlen((char *)filename) - 1] = '\0';
                    status = ledger->load_from((void **)&ledger, (const char *)filename);
                }
                break;
            case DISPLAY:
                ledger->display((void **)&ledger);
                break;
            case USAGE:
                display_options();
                break;
            case EXIT:
                goto end;
            default:
                break;
        }

        if (status != OK) {
            PRINT_ERR("Program exited with error!");
            break;
        }

        if (*filename) {
            memset(filename, '\0', sizeof(filename));
            // len = 0;
        }

        status &= ~status;

        printf("Enter the command: ");
    }
end:
    if (ledger)
        destructor((void **)&ledger);

    printf("Exiting. Goodbye!\n");

    return status;
}

void display_options(void)
{
    printf(
            "--------------------------------\n"
            "Press 1 to add a customer\t|\n"
            "---------------------------\n"
            "Press 2 to remove a customer\t|\n"
            "---------------------------\n"
            "Press 3 to find a customer\t|\n"
            "---------------------------\n"
            "Press 4 to display customers\t|\n"
            "---------------------------\n"
            "Press 5 to load from file\t|\n"
            "---------------------------\n"
            "Press 6 to display options\t|\n"
            "---------------------------\n"
            "Press 7 to exit\t\t\t|\n"
            "--------------------------------\n");
}

