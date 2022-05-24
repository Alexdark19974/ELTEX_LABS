#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
	pid_t n;

	n = fork();

	if (n == 0)
	{
		printf("I am a child. My PID is %d and my PPID is %d\n", getpid(), getppid());
	}
	else
	{
		printf("I am a parent. My PID is %d and my PPID is %d and my child ID is %d\n", getpid(), getppid(), n);
	}

	return 0;
}
