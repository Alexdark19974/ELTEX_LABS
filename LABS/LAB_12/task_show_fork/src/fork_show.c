#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	pid_t n = fork();

	if (n) {
		printf("PID=%d, PPID=%d, CHID=%d\n", getpid(), getppid(), n);
		// parent process must wait for the child's exit status because it will otherwise become a zombie
		// an unprocessed child's exit status wastes a unique PID number in global PID table 
		wait(NULL); // if arg is NULL, we do not care about the child's return status
	}
	else
		printf("ret = %d, PID=%d, PPID=%d\n", n, getpid(), getppid());

	return 0;
}
