#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define NAME_SIZE 25
#define MIDDLE_NAME_SIZE 25
#define LAST_NAME_SIZE 25
#define NUMBER_SIZE 25

struct ledger
    {
        char *name;
        char *middle_name;
        char *last_name;
        char *number;
    };

void address_book (void);

int main (void)
{
    address_book();
    return 0;
}

void address_book(void)
{
    enum { CUSTOMER = '1', LIST, FIND_CUSTOMER, COMMAND_LIST, DELETE_CUSTOMER, EXIT };
    struct ledger *file = NULL;
    int c = 0;
    int max = 0;
    int add_count = 0;

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

                char n[1] = {0};

                if (max)
                {
                    puts("How many more customers would you like to add?");

                    printf("Enter the number and press \"Enter\": ");

                    static int add_more_memory = 0;

                    n[0] = getchar();
                    n[1] = getchar();
                    add_more_memory = atoi(n);

                    if (!add_more_memory || add_more_memory < 0)
                    {
                        puts("you can't add 0 customers or a negative value");puts("please, enter the value greater than 0");
                        break;
                    }

                    max += add_more_memory;

                    file = (struct ledger *) realloc(file, (sizeof(struct ledger) * max));

                    if (file == NULL)
                    {
                        perror("Out of memory\n");
                        exit(EXIT_FAILURE);
                    }
		        }
		        else
		        {
		            puts ("How many customers would you like to add?");

                    printf("Enter the number and press \"Enter\": ");

		            n[0]= getchar();
		            n[1] = getchar();
		            max = atoi(n);

		            if (max <= 0)
                    {
                        puts("you can't add 0 customers or a negative value");
                        puts("please, enter the value greater than 0");
                        break;
                    }

                    file = (struct ledger *) malloc(sizeof(struct ledger) * max);

                    if (file == NULL)
                    {
                        perror("Out of memory\n");
                        exit(EXIT_FAILURE);
                    }
		        }

                puts("Adding a customer.");

		        for (; add_count < max; add_count++)
		        {
		            printf("1) Name: ");

		            file[add_count].name = (char *) malloc(NAME_SIZE);

                    if (file == NULL)
                    {
                        perror("Out of memory\n");
                        exit(EXIT_FAILURE);
                    }

                    fgets(file[add_count].name, NAME_SIZE, stdin);

                    if (file == NULL)
                    {
                        perror("Out of memory\n");
                        exit(EXIT_FAILURE);
                    }

                    printf("2) Middle name: ");

		            file[add_count].middle_name = (char *) malloc(MIDDLE_NAME_SIZE);

                    if (file == NULL)
                    {
                        perror("Out of memory\n");
                        exit(EXIT_FAILURE);
                    }

                    fgets(file[add_count].middle_name, MIDDLE_NAME_SIZE, stdin);

                    printf("3) Last name: ");

		            file[add_count].last_name = (char *) malloc(LAST_NAME_SIZE);

                    if (file == NULL)
                    {
                        perror("Out of memory\n");
                        exit(EXIT_FAILURE);
                    }

                    fgets(file[add_count].last_name, LAST_NAME_SIZE, stdin);

                    printf("4) Phone number: ");

		            file[add_count].number = (char *) malloc(NUMBER_SIZE);

                    if (file == NULL)
                    {
                        perror("Out of memory\n");
                        exit(EXIT_FAILURE);
                    }

                    fgets(file[add_count].number, NUMBER_SIZE, stdin);
                }

                puts ("Succesfully added");
                break;
            }
            case LIST:
            {
                getchar();

                for (int i = 0; i < max; i++)
                {
		            if (file[i].name == NULL)
		            {	    puts("hole");
			            continue;
		            }

                    printf("1) Name: %s", file[i].name);
                    printf("2) Middle name: %s", file[i].middle_name);
                    printf("3) Last name: %s", file[i].last_name);
                    printf("4) Phone number: %s", file[i].number);
		            puts("");
                }

		        printf("the number of customers: %d\n", max);
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

                enum{ SEARCH_BY_NAME = '1', SEARCH_BY_MIDDLE_NAME, SEARCH_BY_LAST_NAME, SEARCH_BY_PHONE_NUMBER };

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

                        for (int i = 0; i != max; i++)
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

                        fgets(find_middle_name, sizeof(find_middle_name),stdin);

                        for (int i = 0; i != max; i++)
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

                        printf("Enter the last name to be found: ");

                        fgets(find_last_name, sizeof(find_last_name), stdin);

                        for (int i = 0; i != max; i++)
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

                        for (int i = 0; i != max; i++)
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

		        printf ("Enter the command: ");
                continue;
            }
            case DELETE_CUSTOMER:
            {
                getchar();

		        enum{ DELETE_BY_NAME = '1', DELETE_BY_MIDDLE_NAME, DELETE_BY_LAST_NAME, DELETE_BY_PHONE_NUMBER, DELETE_ALL, };
		        int i = 0;

		        puts("Press 1 to delete by name, press 2 - to delete by middle name, press 3 to delete by last name, press 4 to delete by phone number: ");

                puts("hint: if you want to go nuclear and delete everybody, type \"5\".");

		        switch(getchar())
		        {
		            case DELETE_BY_NAME:
		            {
		                getchar();

			            char delete_name[NAME_SIZE] = {0};
			            int match_name = -1;

			            printf("Enter the name to be deleted: ");
			            fgets(delete_name, sizeof(delete_name), stdin);

			            for (i = 0; i < max; i++)
                        {
                            if (file[i].name == NULL)
                            {
                                continue;
                            }
                            if ((match_name = strcmp(file[i].name, delete_name))== 0)
                            {
                                free(file[i].name);
                                file[i].name = NULL;

                                free(file[i].middle_name);
                                file[i].middle_name = NULL;

                                free(file[i].last_name);
                                file[i].last_name = NULL;

                                free(file[i].number);
                                file[i].number = NULL;
                                break;
                            }
			            }

                        for (int j = 0; j < max; j++)
                        {
                            for (int l = 0; l < max; l++)
                            {
                                if (j < l && file[j].name == NULL && file[l].name != NULL)
                        		{
                                    file[j].name = (char *) malloc(NAME_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

		                            file[j].middle_name = (char *) malloc(MIDDLE_NAME_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

                		            file[j].last_name = (char *) malloc(LAST_NAME_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

		                            file[j].number = (char *) malloc(NUMBER_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

                		            strncpy(file[j].name, file[l].name, NAME_SIZE - 1);
                           		    file[j].name[NAME_SIZE - 1] = '\0';

 		                            strncpy(file[j].middle_name, file[l].middle_name, MIDDLE_NAME_SIZE - 1);
                		            file[j].middle_name[MIDDLE_NAME_SIZE - 1] = '\0';

                           		    strncpy(file[j].last_name, file[l].last_name, LAST_NAME_SIZE - 1);
                           		    file[j].last_name[LAST_NAME_SIZE - 1] = '\0';

                           		    strncpy(file[j].number, file[l].number, NUMBER_SIZE - 1);
                            		file[j].name[NUMBER_SIZE - 1] = '\0';

                            		free(file[l].name);
                            		file[l].name = NULL;

                            	    free(file[l].middle_name);
                            		file[l].middle_name = NULL;

                            		free(file[l].last_name);
                           		    file[l].last_name = NULL;

                           		    free(file[l].number);
                           		    file[l].number = NULL;
                                    break;
                        		}
				            }
                        }

                        max--;
                        add_count--;
                        i = 0;

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

		            case DELETE_BY_MIDDLE_NAME:
		            {
			            getchar();

			            char delete_middle_name[MIDDLE_NAME_SIZE] = {0};
			            int match_middle_name = -1;

		    	        printf("Enter the middle name to be deleted: ");
                        fgets(delete_middle_name, sizeof(delete_middle_name), stdin);

			            for (i = 0; i != max; i++)
                        {
                            if (file == NULL)
                            {
                                continue;
                            }
                            if ((match_middle_name = strcmp(file[i].middle_name, delete_middle_name)) == 0)
                            {
                                free(file[i].name);
                                file[i].name = NULL;

                                free(file[i].middle_name);
                                file[i].middle_name = NULL;

                                free(file[i].last_name);
                                file[i].last_name = NULL;

                                free(file[i].number);
                                file[i].number = NULL;
                                break;
                            }
			            }

                        for (int j = 0; j < max; j++)
                        {
                            for (int l = 0; l < max; l++)
                            {
                                if (j < l && file[j].name == NULL && file[l].name != NULL)
                        		{
                                    file[j].name = (char *) malloc(NAME_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

		                            file[j].middle_name = (char *) malloc(MIDDLE_NAME_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

                		            file[j].last_name = (char *) malloc(LAST_NAME_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

		                            file[j].number = (char *) malloc(NUMBER_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

                		            strncpy(file[j].name, file[l].name, NAME_SIZE - 1);
                           		    file[j].name[NAME_SIZE - 1] = '\0';

 		                            strncpy(file[j].middle_name, file[l].middle_name, MIDDLE_NAME_SIZE - 1);
                		            file[j].middle_name[MIDDLE_NAME_SIZE - 1] = '\0';

                           		    strncpy(file[j].last_name, file[l].last_name, LAST_NAME_SIZE - 1);
                           		    file[j].last_name[LAST_NAME_SIZE - 1] = '\0';

                           		    strncpy(file[j].number, file[l].number, NUMBER_SIZE - 1);
                            		file[j].name[NUMBER_SIZE - 1] = '\0';

                            		free(file[l].name);
                            		file[l].name = NULL;

                            	    free(file[l].middle_name);
                            		file[l].middle_name = NULL;

                            		free(file[l].last_name);
                           		    file[l].last_name = NULL;

                           		    free(file[l].number);
                           		    file[l].number = NULL;
                                    break;
                        		}
				            }
                        }

                        max--;
                        add_count--;
                        i = 0;

                        if (match_middle_name == -1)
                        {
                            puts("The middle name not found.");
                        }
                        else
                        {
                            puts("Successfully deleted.");
                        }

                        break;
		            }
		            case DELETE_BY_LAST_NAME:
                    {
			            getchar();

			            char delete_last_name[LAST_NAME_SIZE] = {0};
			            int match_last_name = -1;

			            printf("Enter the last name to be deleted: ");
                        fgets(delete_last_name, sizeof(delete_last_name),stdin);

    			        for (i = 0; i != max; i++)
                        {
                            if (file == NULL)
                            {
                                continue;
                            }
                            if ((match_last_name = strcmp(file[i].last_name, delete_last_name)) == 0)
                            {
                                free(file[i].name);
                                file[i].name = NULL;

                                free(file[i].middle_name);
                                file[i].middle_name = NULL;

                                free(file[i].last_name);
                                file[i].last_name = NULL;

                                free(file[i].number);
                                file[i].number = NULL;
                                break;
                            }
	    		        }

                        for (int j = 0; j < max; j++)
                        {
                            for (int l = 0; l < max; l++)
                            {
                                if (j < l && file[j].name == NULL && file[l].name != NULL)
                        		{
                                    file[j].name = (char *) malloc(NAME_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

		                            file[j].middle_name = (char *) malloc(MIDDLE_NAME_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

                		            file[j].last_name = (char *) malloc(LAST_NAME_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

		                            file[j].number = (char *) malloc(NUMBER_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

                		            strncpy(file[j].name, file[l].name, NAME_SIZE - 1);
                           		    file[j].name[NAME_SIZE - 1] = '\0';

 		                            strncpy(file[j].middle_name, file[l].middle_name, MIDDLE_NAME_SIZE - 1);
                		            file[j].middle_name[MIDDLE_NAME_SIZE - 1] = '\0';

                           		    strncpy(file[j].last_name, file[l].last_name, LAST_NAME_SIZE - 1);
                           		    file[j].last_name[LAST_NAME_SIZE - 1] = '\0';

                           		    strncpy(file[j].number, file[l].number, NUMBER_SIZE - 1);
                            		file[j].name[NUMBER_SIZE - 1] = '\0';

                            		free(file[l].name);
                            		file[l].name = NULL;

                            	    free(file[l].middle_name);
                            		file[l].middle_name = NULL;

                            		free(file[l].last_name);
                           		    file[l].last_name = NULL;

                           		    free(file[l].number);
                           		    file[l].number = NULL;
                                    break;
                        		}
				            }
                        }

                        max--;
                        add_count--;
                        i = 0;

			            if (match_last_name == -1)
                        {
                            puts("The last name not found.");
                        }
                        else
                        {
                            puts("Successfully deleted.");
                        }
                        break;
		            }
		            case DELETE_BY_PHONE_NUMBER:
		            {
			            getchar();

			            char delete_number[NUMBER_SIZE] = {0};
			            int match_phone = -1;

			            printf("Enter the number to be deleted: ");
                        fgets(delete_number, sizeof(delete_number), stdin);

	    		        for (i = 0; i != max; i++)
                        {
                            if (file == NULL)
                            {
                        	    continue;
            	            }
            	            if ((match_phone = strcmp(file[i].number, delete_number)) == 0)
            	            {
                    	        free(file[i].name);
                    	        file[i].name = NULL;

                    	        free(file[i].middle_name);
	                            file[i].middle_name = NULL;

    	                        free(file[i].last_name);
                                file[i].last_name = NULL;

                	            free(file[i].number);
                        	    file[i].number = NULL;
                       		    break;
                            }
			            }

                        for (int j = 0; j < max; j++)
                        {
                            for (int l = 0; l < max; l++)
                            {
                                if (j < l && file[j].name == NULL && file[l].name != NULL)
                        		{
                                    file[j].name = (char *) malloc(NAME_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

		                            file[j].middle_name = (char *) malloc(MIDDLE_NAME_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

                		            file[j].last_name = (char *) malloc(LAST_NAME_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

		                            file[j].number = (char *) malloc(NUMBER_SIZE);

                                    if (file == NULL)
                                    {
                                        perror("Out of memory\n");
                                        exit(EXIT_FAILURE);
                                    }

                		            strncpy(file[j].name, file[l].name, NAME_SIZE - 1);
                           		    file[j].name[NAME_SIZE - 1] = '\0';

 		                            strncpy(file[j].middle_name, file[l].middle_name, MIDDLE_NAME_SIZE - 1);
                		            file[j].middle_name[MIDDLE_NAME_SIZE - 1] = '\0';

                           		    strncpy(file[j].last_name, file[l].last_name, LAST_NAME_SIZE - 1);
                           		    file[j].last_name[LAST_NAME_SIZE - 1] = '\0';

                           		    strncpy(file[j].number, file[l].number, NUMBER_SIZE - 1);
                            		file[j].name[NUMBER_SIZE - 1] = '\0';

                            		free(file[l].name);
                            		file[l].name = NULL;

                            	    free(file[l].middle_name);
                            		file[l].middle_name = NULL;

                            		free(file[l].last_name);
                           		    file[l].last_name = NULL;

                           		    free(file[l].number);
                           		    file[l].number = NULL;
                                    break;
                        		}
				            }
                        }

                        if (match_phone == -1)
                        {
                            puts("The phone number not found.");
                        }
                        else
                        {
                            puts("Successfully deleted.");
			    max--;
                            add_count--;
                            i = 0;
                        }

                        break;
                    }
                    case DELETE_ALL:
                    {
                        getchar();
			puts("let's go nuclear! Deleting everybody");

                        for (int m = 0; m < max; m++)
                        {
                            free(file[m].name);
                            file[m].name = NULL;

                            free(file[m].middle_name);
                            file[m].middle_name = NULL;

                            free(file[m].last_name);
                            file[m].last_name = NULL;

                            free(file[m].number);
                            file[m].number = NULL;

                        }

			puts("successfully deleted");
			max = 0;
                        break;
                    }
                }

		        break;
            }
            case EXIT:
            {
		        for (int j = 0; j != max; j++)
		        {
		            free(file[j].name);
		            file[j].name = NULL;

		            free(file[j].middle_name);
                    file[j].middle_name = NULL;

		            free(file[j].last_name);
                    file[j].last_name = NULL;

		            free(file[j].number);
                    file[j].number = NULL;
                }

                free(file);
                file = NULL;

                puts("Closing the address book.");
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
