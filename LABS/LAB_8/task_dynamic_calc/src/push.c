#include "calculator.h"
#include "stack.h"
#include "debug.h"

void push(StackP self, double item)
{

    if (self->idx >= ARRAYSIZE) {
        PRINT_DBG("[%s:%d] stack is full\n", __func__, __LINE__);
        PRINT_DBG(
            "[%s:%d]numbers of items in stack:"
            "%zu, max = %d",
            __func__, __LINE__,
            self->idx,
            ARRAYSIZE
        );

        return;
    }

    self->array[self->idx++] = item;
}
