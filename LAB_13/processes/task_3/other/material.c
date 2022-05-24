#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int i = 1;

	printf("program: %s\n", argv[0]);

	printf("Программа запущена и получила строку: ");

	while (argv[i] != NULL)
	{
		printf("%s\n", argv[i]);
		i++;
	}

	exit (EXIT_SUCCESS);
}
