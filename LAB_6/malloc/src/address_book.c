#include "address_book.h"

void address_book(void)
{
    LedgerP head = NULL;
    int c = 0;

    puts("--------the address book---------");
    puts("Press 4 to list commands");
    puts("Enter the command:");

    while ((c = getchar()) != EOF) {
        clear_stdin_buffer();
        switch (c - '0') {
            case ADD_CUSTOMER:
                add_customer(&head);
                break;
            case DISPLAY_CUSTOMERS:
                display_customer(&head);
                break;
            case FIND_CUSTOMER:
                find_customer(&head);
                break;
            case REMOVE_CUSTOMER:
                remove_customer(&head);
                break;
            case DISPLAY_OPTIONS:
                display_options();
                break;
            case EXIT:
                free_all(&head);
                return;
        }
        printf("Enter the command: ");
    }
    free_all(&head);
}
