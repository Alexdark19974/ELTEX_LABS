#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "address_book.h"

static void display_options(void);

unsigned char debug = 0;

Ledger ledger = {
    /* Assign pointers to functions to null pointers */
    &add,       // add()
    &remove_,   // remove_()
    &find,      // find()
    &display,   // display()

    /* Designated initializer is from C99 onwards */
    {
        {
            {0},    // name

            {0},    // middle_name

            {0},    // last_name

            {0},    // phone_number

            {0},    // occupation

            0       // age
        }
    }
};

int address_book(void)
{
    int c = 0, status = EXIT_SUCCESS;

#ifdef DEBUG
    debug = 1;
    puts("Debug mode ON!");
#endif

    puts("-------------the address book--------------");
    puts("Press 5 to list commands");
    printf("Enter the command: ");

    while ((c = getchar()) != EOF) {

        if (!isspace(c))
            clear_stdin_buffer();

        switch (c - '0') {
            case ADD:
                status = ledger.add();
                break;
            case REMOVE:
                status = ledger.remove_();
                break;
            case FIND:
                status = ledger.find();
                break;
            case DISPLAY:
                ledger.display();
                break;
            case USAGE:
                display_options();
                break;
            case EXIT:
                goto end;
            default:
                printf("Unknown command!\n");
        }

        if (status == ERR) {
            status = EXIT_FAILURE;
            goto end;
        }

        printf("Enter the command: ");
    }

end:
    return status;
}

void display_options(void)
{
    printf(
        "Press 1 to add a customer\n"
        "Press 2 to remove a customer\n"
        "Press 3 to find a customer\n"
        "Press 4 to display customers \n"
        "Press 5 to display usage\n"
        "Press 6 to exit\n"
    );
}
