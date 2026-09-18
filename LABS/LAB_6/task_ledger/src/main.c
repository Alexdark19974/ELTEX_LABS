#include <stdlib.h>
#include "address_book.h"

int main(void)
{
    if (address_book() == -1)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
