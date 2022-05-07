#include "calc_hub.h"

int const id = CALC;

double my_calc (void)
{
	enum
	{
		DIV_FUNC = 1,
		ADD_FUNC,
		SUB_FUNC,
		MUL_FUNC,
		GET_LINE_FUNC,
		STACK_POP_FUNC
	};

    char buf_for_calc[MAX_LINE] = {0};
    char *path_arr[PATH_ARR_SIZE] = {NULL};
    void *lib_handler[HANDLER_SIZE] = {NULL};
    struct dirent *entry_ptr = NULL;
    double res = 0;
    double stack[MAX_LINE] = {0.0};
    int counter = 0;
	int len = 0;
    char token = 0;

    func_add_ptr add = NULL;
    func_sub_ptr sub = NULL;
    func_div_ptr _div = NULL;
    func_mul_ptr mul = NULL;
    func_ptr calc = NULL;
    func_line_ptr my_get_line = NULL;
    func_stack_ptr push_into_stack = NULL;
    func_pop_ptr pop_stack = NULL;
    DIR *dir_ptr = NULL;

    dir_ptr = opendir("./libs");

	if (!dir_ptr)
	{
		fprintf(stderr, "can't open \"./libs\".Abort.\n");
        exit(EXIT_FAILURE);
	}

    for (int i = 0; (entry_ptr = readdir(dir_ptr)) != NULL;)
    {
        if ((strstr(entry_ptr->d_name, ".so")) != NULL)
        {
            path_arr[i] = malloc(MALLOC_BUF);
            strncpy(path_arr[i], "./libs/", CPY_LIMIT);
			len = strlen(path_arr[i]);
            path_arr[i][len] = '\0';

            strcat(path_arr[i], entry_ptr->d_name);

            lib_handler[i] = dlopen(path_arr[i], RTLD_NOW);

            if (!lib_handler[i])
            {
                fprintf(stderr, "%s\n", dlerror());
                exit(EXIT_FAILURE);
            }

           int *token = dlsym(lib_handler[i], "id");

            switch (*token)
            {
                case DIV_FUNC:
                {
                    _div = dlsym(lib_handler[i], "_div");

					if (!_div)
            		{
                		fprintf(stderr, "%s\n", dlerror());
                		exit(EXIT_FAILURE);
            		}
                    break;
                }
                case ADD_FUNC:
                {
                    add = dlsym(lib_handler[i], "add");

					if (!add)
                    {
                        fprintf(stderr, "%s\n", dlerror());
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
                case SUB_FUNC:
                {
                    sub = dlsym(lib_handler[i], "sub");

					if (!sub)
                    {
                        fprintf(stderr, "%s\n", dlerror());
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
                case MUL_FUNC:
                {
                    mul = dlsym(lib_handler[i], "mul");

					if (!mul)
                    {
                        fprintf(stderr, "%s\n", dlerror());
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
                case GET_LINE_FUNC:
                {
                    my_get_line = dlsym(lib_handler[i], "my_get_line");

					if (!my_get_line)
                    {
                        fprintf(stderr, "%s\n", dlerror());
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
                case STACK_POP_FUNC:
                {
                    push_into_stack = dlsym(lib_handler[i], "push_into_stack");
                    pop_stack = dlsym(lib_handler[i],"stack_pop");

					if (!push_into_stack || !pop_stack)
                    {
                        fprintf(stderr, "%s\n", dlerror());
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
                default:
                {
                    printf("Wrong id. Abort.\n");
                    exit(EXIT_FAILURE);
                }
            }

            i++;
        }
    }

    printf ("Insert data to be calculated: ");

    while ((token = my_get_line(buf_for_calc, MAX_LINE)))
    {
        switch (token)
        {
            case MY_EOF:
            {
				for (int i = 0; path_arr[i]; i++)
				{
					free(path_arr[i]);
				}
				for (int i = 0; lib_handler[i]; i++)
				{
					dlclose(lib_handler[i]);
				}
				closedir(dir_ptr);
                return EOF;
            }
            case NUMBER:
            {
                push_into_stack(stack, buf_for_calc, &counter);
                break;
            }
            case '+':
            {
                if (add)
                {
                    res = add(stack, &counter, pop_stack);
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
                    res = sub(stack, &counter, pop_stack);
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
                    res = mul(stack, &counter, pop_stack);
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
                    res = _div(stack, &counter, pop_stack);
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
                printf("= %lf\n\n", res);
				printf ("Insert data to be calculated: ");
				break;
            }
            case '\n':
            {
                printf("= %lf\n\n", res);
				printf ("Insert data to be calculated: ");
				break;
            }
            default:
            {
                printf ("error: unknown command\n");

				for (int i = 0; path_arr[i]; i++)
                {
                    free(path_arr[i]);
                }
                for (int i = 0; lib_handler[i]; i++)
                {
                    dlclose(lib_handler[i]);
                }
				closedir(dir_ptr);
                exit(EXIT_FAILURE);
            }
        }
    }

    return token;
}

