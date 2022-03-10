#include <stdio.h>
#include <string.h>
#define NAME_SIZE 50
#define MIDDLE_NAME_SIZE 50
#define LAST_NAME_SIZE 50
#define NUMBER_SIZE 50
#define MAX_ENTRY 10

struct ledger
    {
        char name[NAME_SIZE];
        char middle_name[MIDDLE_NAME_SIZE];
        char last_name[LAST_NAME_SIZE];
        char number[NUMBER_SIZE];
    };

void address_book (void);

int main (void)
{
    address_book();
    return 0;
}

void address_book(void)
{
    enum {CUSTOMER = '1', LIST, FIND_CUSTOMER, COMMAND_LIST, DELETE_CUSTOMER, EXIT};
    struct ledger file[MAX_ENTRY];
    int c = 0;
    int counter = 0;

    puts("You have opened the address book.");
    puts("Press 4 to list commands");
    puts("Enter the command:");

    while ((c = getchar()) != EOF)
    {
        switch (c)
        {
            case CUSTOMER:
            {
                getchar();

                puts("Adding a customer.");

                printf("1) Name: ");

                fgets(file[counter].name, sizeof(file[counter].name), stdin);

                printf("2) Middle name: ");

                fgets(file[counter].middle_name, sizeof(file[counter].middle_name), stdin);

                printf("3) Last name: ");

                fgets(file[counter].last_name, sizeof(file[counter].last_name), stdin);

                printf("4) Phone number: ");

                fgets(file[counter].number, sizeof(file[counter].middle_name), stdin);

                counter++;

                puts ("Succesfully added");
                break;
            }
            case LIST:
            {
                getchar();

                for (int i = 0; i != counter; i++)
                {
                    printf("1) Name: %s", file[i].name);
                    printf("2) Middle name: %s", file[i].middle_name);
                    printf("3) Last name: %s", file[i].last_name);
                    printf("4) Phone number: %s", file[i].number);
                }
                break;
            }
            case COMMAND_LIST:
            {
                getchar();

                puts("1 - add a customer, 2 - list all customers, 3 - find a customer, 4 - list all commands, 5 - delete a customer, 6 - exit the ledger");
                break;
            }
            case FIND_CUSTOMER:
            {
                getchar();

                enum{SEARCH_BY_NAME = '1', SEARCH_BY_MIDDLE_NAME, SEARCH_BY_LAST_NAME, SEARCH_BY_PHONE_NUMBER};

                puts("Press 1 to search by name, press 2 - to search by middle name, press 3 to search by last name, press 4 to search by phone number: ");

                switch (getchar())
                {
                    case SEARCH_BY_NAME:
                    {
                        getchar();

                        char find_name[50] = {0};
                        int found_name = -1;

                        printf("Enter the name to be found: ");

                        fgets(find_name, sizeof(find_name), stdin);

                        for (int i = 0; i != counter; i++)
                        {
                            if ((found_name = (strcmp(file[i].name, find_name))) == 0)
                            {
                                printf("1) Name: %s", file[i].name);
                                printf("2) Middle name: %s", file[i].middle_name);
                                printf("3) Last name: %s", file[i].last_name);
                                printf("4) Phone number: %s", file[i].number);
                                break;
                            }
                        }
                        
                        if (found_name != 0)
                        {
                            puts("The name not found.");
                        }
                        else
                        {
                            puts("Successfully found.");
                        }

                        break;
                    }
                    case SEARCH_BY_MIDDLE_NAME:
                    {
                        getchar();

                        char find_middle_name[50] = {0};
                        int found_middle_name = -1;
                        
                        printf("Enter the middle name to be found: ");

                        fgets(find_middle_name, sizeof(find_middle_name), stdin);

                        for (int i = 0; i != counter; i++)
                        {
                            if ((found_middle_name = (strcmp(file[i].middle_name, find_middle_name))) == 0)
                            {
                                printf("1) Name: %s", file[i].name);
                                printf("2) Middle name: %s", file[i].middle_name);
                                printf("3) Last name: %s", file[i].last_name);
                                printf("4) Phone number: %s", file[i].number);
                                break;
                            }
                        }
                        
                        if (found_middle_name != 0)
                        {
                            puts("The middle name not found.");
                        }
                        else
                        {
                            puts("Successfully found.");
                        }

                        break;
                    }
                    case SEARCH_BY_LAST_NAME:
                    {
                        getchar();

                        char find_last_name[50] = {0};
                        int found_last_name = -1;
                        
                        printf("Enter the middle name to be found: ");

                        fgets(find_last_name, sizeof(find_last_name), stdin);

                        for (int i = 0; i != counter; i++)
                        {
                            if ((found_last_name = (strcmp(file[i].last_name, find_last_name))) == 0)
                            {
                                printf("1) Name: %s", file[i].name);
                                printf("2) Middle name: %s", file[i].middle_name);
                                printf("3) Last name: %s", file[i].last_name);
                                printf("4) Phone number: %s", file[i].number);
                                break;
                            }
                        }
                        
                        if (found_last_name != 0)
                        {
                            puts("The last name not found.");
                        }
                        else
                        {
                            puts("Successfully found.");
                        }

                        break;
                    }
                    case SEARCH_BY_PHONE_NUMBER:
                    {
                        getchar();

                        char find_phone_number[50] = {0};
                        int found_phone_number = -1;
                        
                        printf("Enter the phone number name to be found: ");

                        fgets(find_phone_number, sizeof(find_phone_number), stdin);

                        for (int i = 0; i != counter; i++)
                        {
                            if ((found_phone_number = (strcmp(file[i].number, find_phone_number))) == 0)
                            {
                                printf("1) Name: %s", file[i].name);
                                printf("2) Middle name: %s", file[i].middle_name);
                                printf("3) Last name: %s", file[i].last_name);
                                printf("4) Phone number: %s", file[i].number);
                                break;
                            }
                        }
                        
                        if (found_phone_number != 0)
                        {
                            puts("The phone number not found.");
                        }
                        else
                        {
                            puts("Successfully found.");
                        }

                        break;
                    }
                }
            }
            case DELETE_CUSTOMER:
            {
                getchar();

                char delete_name[50] = {0};
                int match_name = -1;

                printf("Enter the name to be deleted: ");

                fgets(delete_name, sizeof(delete_name), stdin);

                for (int i = 0; i != counter; i++)
                {
                    if ((match_name = strcmp(file[i].name, delete_name)) == 0)
                    {
                        for (int j = 0; file[i].name[j] != 0 
                            || file[i].middle_name[j] != 0 
                            || file[i].last_name[j] != 0
                            || file[i].number[j] != 0; j++)
                        {
                            file[i].name[j] = 0;
                            file[i].middle_name[j] = 0;
                            file[i].last_name[j] = 0;
                            file[i].number[j] = 0;
                        }
                        break;
                    }
                }

                if (match_name == -1)
                {
                    puts("The name not found.");
                }
                else
                {
                    puts("Successfully deleted.");
                }

                break;
            }
            case EXIT:
            {
                puts("Closing the address book");
                puts("Goodbye.");
                return;
            }
            default:
            {
                getchar();
                printf ("error: invalid command\n");
                break;
            }
        }
        printf("Enter the command: ");
    }
}