#include "common.h"
#include <stdbool.h>
#include <fcntl.h>
#include <getopt.h>
#include "fopen.h"
#include "open.h"
#include "debug.h"

static struct option longopts[] =
{
	{
		"fopen",
		no_argument,
		NULL,
		'f'
	},

	{
		"open",
		no_argument,
		NULL,
		'o'
		
	},

	{
		NULL,
		no_argument,
		NULL,
		0
	}
};

/* Create a file, write a string to it, then reopen it and read it from the end. */

int main(int argc, char **argv)
{
	int c = 0, mode = 0, status = OK;

	while (true) {

		c = getopt_long(argc, argv, "fo", longopts, NULL);

		if (c == -1) {
			if (errno) {
				PRINT_ERR("error while parsing args: %s", strerror(errno));
				errno = 0;
				goto fail;
			}
			PRINT_DBG("%s", "Done parsing args");
			break;
		}

		switch(c) {
			case FOPEN_MODE:;
			case OPEN_MODE:
				mode = c;
				PRINT_DBG("Selected '%s' mode", c == FOPEN_MODE ? "fopen" : "open");
				break;
			default:
				if (c == UNKNOWN_MODE) {
					PRINT_ERR("error while parsing args: unknown option (%c:%#x)", c, c);
					status = ERR;
					goto fail;
				}
				break;
		}
	};

	if (0 == mode) {
		PRINT("%s", "No mode specified. Defaulting to fopen mode");
		mode = FOPEN_MODE;
	}

	PRINT_DBG("opening file in %s mode", mode == FOPEN_MODE ? "fopen" : "open");

	status = (mode == OPEN_MODE) ? open_mode() : fopen_mode();

fail:
	return status;	
}

