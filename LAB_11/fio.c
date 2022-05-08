#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define PERMS 0666
#define STDOUT 1
#define BYTE 1
#define ERROR -1
/* создать файл, записать строку в него, вторично открыть файл и прочитать строку с конца */

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "argv lacks parameters for the program to work properly\n");
		exit(EXIT_FAILURE);
	}

	char c = 0;

	int fd = creat(argv[1], PERMS);

	int len = strlen(argv[2]);

	int n = write(fd, argv[2], len);

	if (n != (len))
	{
		fprintf(stderr, "error: mismatch of len and n\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		close(fd);
	}

	fd = open(argv[1], O_RDONLY, 0);

	if (fd == ERROR)
	{
		fprintf(stderr, "error: can't open file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	for (int i = -1; i >= (len * (-1)); i--)
	{
		lseek(fd, i, SEEK_END);

		n = read(fd, &c, BYTE);

		if (n != BYTE)
		{
			fprintf(stderr, "error: can't read file %s\n", argv[1]);
			exit(EXIT_FAILURE);
		}

    	write(STDOUT, &c, BYTE);
	}

	c = '\n';

	write(STDOUT, &c, BYTE);

	exit(EXIT_SUCCESS);
}
