#include "calc_hub.h"

int main(int argc, char *argv[])
{
    double res = 0;
	int len = 0;
	void *lib_calc_handler = NULL;
	char *path_calc = NULL;
	func_ptr my_calc = NULL;
	DIR *dir_ptr = NULL;
	struct dirent *entry_ptr = NULL;

	dir_ptr = opendir("./calc_lib");

	if (!dir_ptr)
	{
		fprintf(stderr, "can't open \"./calc_lib\".Abort.\n");
		exit(EXIT_FAILURE);
	}

	for (;(entry_ptr = readdir(dir_ptr)) != NULL;)
	{
		if ((strstr(entry_ptr->d_name, ".so")) != NULL)
		{
			path_calc = malloc(BUFSIZE);

    		strncpy(path_calc, "./calc_lib/", CPY_LIMIT);
			len = strlen(path_calc);
    		path_calc[len] = '\0';
	    	strcat(path_calc, entry_ptr->d_name);

			lib_calc_handler = dlopen(path_calc, RTLD_NOW);

			if (!lib_calc_handler)
			{
				fprintf(stderr, "%s\n", dlerror());
               	exit(EXIT_FAILURE);
			}

			int *num = dlsym(lib_calc_handler, "id");

			if (CALC == (*num))
			{
				my_calc = dlsym(lib_calc_handler, "my_calc");
			}
			else
			{
				fprintf(stderr, "wrong id.\n");
				exit(EXIT_FAILURE);
			}

			if (!my_calc)
			{
				fprintf(stderr, "%s\n", dlerror());
				exit(EXIT_FAILURE);
			}

			break;
		}
	}

	printf("--------Plug-In Calculator--------\n\n");

    while ((res = my_calc()) != EOF)
    {
    	;
    }

	free(path_calc);
	dlclose(lib_calc_handler);
	closedir(dir_ptr);

    exit(EXIT_SUCCESS);
}
