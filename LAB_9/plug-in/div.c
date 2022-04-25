#include "calc_hub.h"

double _div(double *stack, int *counter)
{
	void* lib_stack_handler = NULL;
    lib_stack_handler = dlopen("./libmy_stack.so", RTLD_LAZY);

    if (!lib_stack_handler)
    {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    typedef double (*func_ptr)(double *, int *);

    func_ptr pop_ptr = dlsym(lib_stack_handler,"stack_pop");

	double digit_2 = pop_ptr(stack, counter);
   	double digit_1 = pop_ptr(stack, counter);

   	double res = digit_1 / digit_2;

    return res;
}



