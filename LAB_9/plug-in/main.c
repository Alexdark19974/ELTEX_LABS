#include "calc_hub.h"

int main (void)
{
    double res = 0;

	static char intrpt = 0;

	void *lib_handler = NULL;

	typedef double (*func_ptr)(void);

	printf("--------Plug-In Calculator--------\n\n");

	lib_handler = dlopen("./libmy_calc.so", RTLD_LAZY);

	if (!lib_handler)
	{
		lib_handler = dlopen("./libmy_calc.so.1.2.0", RTLD_LAZY);

		if (!lib_handler)
		{
			fprintf(stderr, "%s\n", dlerror());
			exit(EXIT_FAILURE);
		}
	}

	if (!intrpt)
	{
		printf("Dynamic library \"libmy_calc.so or versioned libmy_calc.so.1.2.0\" uploaded successfully\n");
		printf("1)\tAs of now, the library contains one function \"calc\"\n");
		printf("2)\tWhich enables the mininal interface to allow the user to upload all necessary libraries at wish.\n");
		intrpt = 1;
	}

	func_ptr calc_ptr = dlsym(lib_handler, "calc");

    while ((res = calc_ptr()) != EOF)
    {
    	;
    }

	dlclose(lib_handler);
	lib_handler = NULL;

    return 0;
}
