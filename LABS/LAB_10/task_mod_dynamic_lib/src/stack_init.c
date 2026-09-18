#include "calculator.h"
#include "stack.h"

void init_stack(StackP stack)
{
    memset(stack, 0, sizeof(Stack));
    stack->push = push;
    stack->pop = pop;
}
