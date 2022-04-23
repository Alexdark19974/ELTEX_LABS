#include "calc_hub.h"

double calc (void)
{
    char buf_for_calc[MAX_LINE] = {0};
	static char intrpt = 0;
    double res = 0;
    double stack[MAX_LINE] = {0.0};
	char *arr_ptr[4] = {NULL};
    int counter = 0;
    char token = 0;
	char c = 0;
	static void *lib_my_get_line_handler = NULL;
	static void *lib_stack_handler = NULL;
	static void *lib_add_handler = NULL;
	static void *lib_sub_handler = NULL;
	static void *lib_div_handler = NULL;
	static void *lib_mul_handler = NULL;
	typedef int (*func_line_ptr) (char *, int);
	typedef double (*func_add_ptr)(double *, int *);
	typedef double (*func_sub_ptr)(double *, int *);
	typedef double (*func_div_ptr)(double *, int *);
	typedef double (*func_mul_ptr)(double *, int *);
	typedef double (*func_stack_ptr)(double *, char *, int *);
	func_add_ptr add = NULL;
	func_sub_ptr sub = NULL;
	func_div_ptr _div = NULL;
	func_mul_ptr mul = NULL;

	lib_my_get_line_handler = dlopen("./libmy_get_line.so", RTLD_LAZY);

	if (!lib_my_get_line_handler)
    {
    	fprintf(stderr, "%s\n", dlerror());
    	exit(EXIT_FAILURE);
    }

	if (!intrpt)
	{
		printf("\nDynamic library \"libmy_get_line.so\" uploaded successfully\n");
		printf("1)\tThe library contains only one function \"my_get_line\"\n");
		printf("2)\tWhich enables the user to input data such as \" 2 2 + = \" or \" 2 2 + \"\n\n");
		intrpt = 1;
	}

	func_line_ptr my_get_line_ptr = dlsym (lib_my_get_line_handler, "my_get_line");

	printf ("Press 1 to see info on uploading libraries, press 2 to check the status of the upload or Press ENTER to proceed: ");

	while ((c = getchar()) != '\n' && c != EOF)
	{
		c = c - 48;

		switch (c)
		{
			case INFO:
			{
				getchar();

				printf("there are 4 libraries you can upload:\n");
	        	printf("1)\t\"libmy_add.so\" (contains function \"add\")\n");
	        	printf("2)\t\"libmy_sub.so\" (contains function \"sub\")\n");
	        	printf("3)\t\"libmy_mul.so\" (contains function \"mul\")\n");
	        	printf("4)\t\"libmy_div.so\" (contains function \"div\")\n\n");
				printf("Keep in mind that the calculator is a bit dumb so you will have to bear with it, OK?\n");
				printf("When typing the name of a library, press ENTER every time to upload it, and then type in another name.\n\n");
				break;
			}
			case STATUS:
			{
				getchar();

				if (lib_add_handler != NULL)
				{
					printf("1)\t\"libmy_add.so\" uploaded;\n");
				}
				else
				{
					printf("1)\t\"libmy_add.so\" missing;\n");
				}

				if (lib_sub_handler != NULL)
				{
					printf("2)\t\"libmy_sub.so\" uploaded;\n");
				}
				else
				{
					printf("2)\t\"libmy_sub.so\" missing;\n");
				}

				if (lib_div_handler != NULL)
				{
					printf("3)\t\"libmy_mul.so\" uploaded;\n");
				}
				else
				{
					printf("3)\t\"libmy_mul.so\" missing;\n");
				}

				if (lib_mul_handler != NULL)
				{
					printf("4)\t\"libmy_div.so\" uploaded;\n");
				}
				else
				{
					printf("4)\t\"libmy_add.so\" missing;\n");
				}
				break;
			}
			case CLOSE_ALL:
			{
				if (lib_add_handler != NULL)
                {
                    dlclose(lib_add_handler);
					lib_add_handler = NULL;

					printf ("Succesfully closed\n");
                }
                else
                {
                    printf("1)\t\"libmy_add.so\" already closed;\n");
                }

                if (lib_sub_handler != NULL)
                {
					dlclose(lib_sub_handler);
                    lib_sub_handler = NULL;

                    printf ("Succesfully closed\n");
                }
                else
                {
					printf("1)\t\"libmy_sub.so\" already closed;\n");
                }

                if (lib_div_handler != NULL)
                {
					dlclose(lib_div_handler);
                    lib_div_handler = NULL;

                    printf ("Succesfully closed\n");
                }
                else
                {
					printf("1)\t\"libmy_div.so\" already closed;\n");
             	}

                if (lib_mul_handler != NULL)
                {
					dlclose(lib_mul_handler);
                    lib_mul_handler = NULL;

                    printf ("Succesfully closed\n");
                }
                else
                {
					printf("1)\t\"libmy_mul.so\" already closed;\n");
                }

				if (lib_stack_handler != NULL)
				{
				   dlclose(lib_stack_handler);
    			   lib_stack_handler = NULL;
				}

                break;
			}
			default:
			{
				getchar();

				printf("Unknown command\n");

				break;
			}
		}
	}

	printf("Choose the number of libraries to be uploaded (from 1 to 4): ");

	while ((c = getchar()) != EOF)
	{
		if (c == '\n')
		{
			printf("error: no number entered\n\n");
			printf("Choose the number of libraries to be uploaded (from 1 to 4): ");
			continue;
		}
		c = c - 48;

		if (c > 4 || c <= 0)
		{
			printf("error: the number of requested libraries should not be more than 4\n\n");
			getchar();
			printf("Choose the number of libraries to be uploaded (from 1 to 4): ");
			continue;
		}

		for (int i = 0; i != c; i++)
    	{
    		arr_ptr[i] = malloc(100);
    	}

		getchar();

		break;
	}

	if (c == EOF)
	{
		return EOF;
	}

	printf("Enter the name(s) of the library(ies):\n");

	for (int i = 0; i != c; i++)
	{
		printf(": ");
		fgets(arr_ptr[i], 100, stdin);
	}

	for (int i = 0; i != c; i++)
	{
		if (strcmp(arr_ptr[i], "libmy_add.so\n") == 0)
		{
			if (!lib_add_handler)
			{
				lib_add_handler = dlopen("./libmy_add.so", RTLD_LAZY);
				printf("Successfully uploaded\n");
			}

			if (!lib_add_handler)
            {
            	fprintf(stderr, "%s\n", dlerror());
                exit(EXIT_FAILURE);
            }

			add = dlsym(lib_add_handler, "add");
		}
		else if(strcmp(arr_ptr[i], "libmy_sub.so\n") == 0)
		{
			if (!lib_sub_handler)
			{
				lib_sub_handler = dlopen("./libmy_sub.so", RTLD_LAZY);
				printf("Successfully uploaded\n");
			}

			if (!lib_sub_handler)
            {
            	fprintf(stderr, "%s\n", dlerror());
                exit(EXIT_FAILURE);
            }

			sub = dlsym(lib_sub_handler, "sub");
		}
		else if (strcmp(arr_ptr[i], "libmy_div.so\n") == 0)
		{
			if (!lib_div_handler)
			{
				lib_div_handler = dlopen("./libmy_div.so", RTLD_LAZY);
				printf("Successfully uploaded\n");
			}

			if (!lib_div_handler)
            {
            	fprintf(stderr, "%s\n", dlerror());
                exit(EXIT_FAILURE);
            }

            _div = dlsym(lib_div_handler, "_div");
		}
		else if (strcmp(arr_ptr[i], "libmy_mul.so\n") == 0)
		{
			if (!lib_mul_handler)
			{
				lib_mul_handler = dlopen("./libmy_mul.so", RTLD_LAZY);
				printf("Successfully uploaded\n");
			}

            if (!lib_mul_handler)
            {
            	fprintf(stderr, "%s\n", dlerror());
                exit(EXIT_FAILURE);
            }

			mul = dlsym(lib_mul_handler, "mul");
		}
		else
		{
			puts("Unknown library");

			for (int i = 0; arr_ptr[i] != NULL; i++)
    		{
        		free(arr_ptr[i]);
        		arr_ptr[i] = NULL;
    		}

			return res;
		}
	}

    printf ("Insert data to be calculated: ");

    while ((token = my_get_line_ptr(buf_for_calc, MAX_LINE)) != EOF)
	{
	    switch (token)
	    {
			case MY_EOF:
			{
				dlclose(lib_my_get_line_handler);
				lib_my_get_line_handler = NULL;

		    	dlclose(lib_stack_handler);
                lib_stack_handler = NULL;

				if (lib_add_handler != NULL)
                {
                    dlclose(lib_add_handler);
                    lib_add_handler = NULL;
                }

				if (lib_sub_handler != NULL)
                {
                    dlclose(lib_sub_handler);
                    lib_sub_handler = NULL;
                }

                if (lib_div_handler != NULL)
                {
                    dlclose(lib_div_handler);
                    lib_div_handler = NULL;
                }

                if (lib_mul_handler != NULL)
                {
                    dlclose(lib_mul_handler);
                    lib_mul_handler = NULL;
	            }

				for (int i = 0; arr_ptr[i] != NULL; i++)
    			{
        			free(arr_ptr[i]);
        			arr_ptr[i] = NULL;
    			}

				return EOF;
			}
			case NUMBER:
			{

				if (!lib_stack_handler)
				{
					lib_stack_handler = dlopen("./libmy_stack.so", RTLD_LAZY);
				}

				if (!lib_stack_handler)
				{
					fprintf(stderr, "%s\n", dlerror());
					exit(EXIT_FAILURE);
				}

				func_stack_ptr stack_push = dlsym(lib_stack_handler, "push_into_stack");
		   		stack_push(stack, buf_for_calc, &counter);
				break;
			}
			case '+':
			{
				if (add)
				{
		    		res = add(stack, &counter);
				}
				else
				{
					printf("error: invalid token; Did you mean another symbol of calculation?\n");
				}

				break;
			}
			case '-':
			{
				if (sub)
				{
        			res = sub(stack, &counter);
				}
				else
				{
					printf("error: invalid token; Did you mean another symbol of calculation?\n");
				}

				break;
			}
			case '*':
			{
				if (mul)
				{
		    		res = mul(stack, &counter);
				}
				else
				{
					printf("error: invalid token; Did you mean another symbol of calculation?\n");
				}

				break;
			}
			case '/':
			{
				if (_div)
				{
		   			res = _div(stack, &counter);
            		break;
				}
				else
				{
					printf("error: invalid token; Did you mean another symbol of calculation? \n");
				}

				break;
			}
			case '=':
			{
		    	getchar();
				printf("= %lf\n", res);
		    	return (res = 0);
			}
			case '\n':
			{
				printf("= %lf\n", res);
				return (res = 0);
			}
			default:
			{
		    	printf ("error: unknown command\n");
		    	exit(-1);
			}
	    }
	}

	for (int i = 0; arr_ptr[i] != NULL; i++)
	{
		free(arr_ptr[i]);
		arr_ptr[i] = NULL;
	}

	return token;
}
