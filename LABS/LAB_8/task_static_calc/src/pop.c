#include "calculator.h"
#include "stack.h"
#include "debug.h"

double pop(StackP self)
{
    if (self->idx == 0) {
        PRINT_DBG("[%s:%d] stack is empty!\n", __func__, __LINE__);
        return EOS; // End of Stack
    }

    return self->array[--self->idx];
}
