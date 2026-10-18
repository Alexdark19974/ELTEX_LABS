#include "common.h"
#include "debug.h"
#include "get_line.h"

int get_line(char *buf, size_t bufsize)
{
	char *ch = NULL;	

	if (!buf || bufsize < 2) {
		PRINT_ERR("%s", "error: either buf is NULL or bufsize is less than 2 bytes long");
		return ERR;
	}

	printf("Enter a string: ");

try:
	if (!fgets(buf, bufsize, stdin) && ferror(stdin)) {
		PRINT_ERR("%s", "error: fgets() failed");
		return ERR;
	}

	if (!*buf || *buf == '\n') {
		PRINT("%s", "no data: try again.");
		goto try;
	}

	if ((ch = strchr(buf, '\n')))
		*ch = '\0';
	
	PRINT("data: %s\n", buf);

	return OK;
}
