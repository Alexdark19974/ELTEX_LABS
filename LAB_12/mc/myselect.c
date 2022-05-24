#include "midc.h"

int myselect(const struct dirent *entry_ptr)
{
    if (strcmp(entry_ptr->d_name, ".") != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
