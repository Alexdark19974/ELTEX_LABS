#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	pid_t n = 0;

//	printf("Hello!\n");

	/* '\n' results in buffer overflow so the line will appear only in the parent process because it will be dumped to the kernel space */

//	printf("Hello!");

	/* no overflow, so the buffer will be copied to the child proccess */

	n = fork();

	if (n == 0)
	{
		printf("Child %d\n", getpid());
	}
	else
	{
		printf("Parent %d\n", getpid());
	}

	return 0;
}
