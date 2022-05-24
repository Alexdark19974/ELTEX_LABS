#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
	pid_t n;
	int status = 0;
	int res = 0;

	n = fork();

	if (n != 0)
	{
		pid_t cid = wait(&status);

		if (status == 0)
		{
			printf(" Success\n Child ID is %d\n", cid);
		}
		else
		{
			printf("failure");
		}
	}
	else
	{
		//res = execl("./material", "material", "hello",  NULL);
		res = execlp("./material", "material", "hello", NULL);

		if (res == -1)
		{
			fprintf(stderr, "can't execute\n");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
