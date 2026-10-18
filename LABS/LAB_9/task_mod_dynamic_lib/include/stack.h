#ifndef STACK_H
#define STACK_H
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>

#define ARRAYSIZE 256
#define EOS 0 // end of stack

struct stack {
    double array[ARRAYSIZE];
    size_t idx;
    void (*push)(StackP self, double item);
    double (*pop)(StackP self);
};

void init_stack(StackP stack);
double pop(StackP self);
void push(StackP self, double item);
#endif /* STACK_H */
