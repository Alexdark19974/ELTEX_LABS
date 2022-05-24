#include "midc.h"

int isDirectory(const char *path)
{
    struct stat sb;

    if (stat(path, &sb) != 0)
    {
        return 0;
    }
    return S_ISDIR(sb.st_mode);
}
