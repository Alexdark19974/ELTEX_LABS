#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
	pid_t n;

	n = fork();

	if (n == 0)
	{
		printf("2) I am a child, my PID is %d and my parent's PPID is %d\n", getpid(), getppid());

		pid_t c;

		c = fork();

		if (c == 0)
		{
			printf("5) I am a child, my PID is %d and my parent's PPID is %d\n", getpid(), getppid());
		}
		else
		{
			printf ("6) I am the second parent, with PID of %d and my child's PID is %d\n", getpid(), c);

			pid_t e;

			e = fork();

			if (e == 0)
			{
				printf("9) I am a child, my PID is %d and my parent's PPID is %d\n", getpid(), getppid());
			}
			else
			{
				printf("10) I am the second parent with PID of %d and my child's PID is %d\n", getpid(), e);
			}
		}
	}
	else
	{
		printf("1) I am the first parent with PID of %d and my first child's PID is %d\n", getpid(), n);

		pid_t a;

		a = fork();

		if (a == 0)
		{
			printf("3) I am a child, my PID is %d and my parent's PID is %d\n", getpid(), getppid());

			pid_t b;

			b = fork();

			if (b == 0)
			{
				printf("7) I am a child, my PID is %d and my paren't PPID is %d\n", getpid(), getppid());
			}
			else
			{
				printf("8) I am the third parent with PID of %d and my child's PID is %d\n", getpid(), b);
			}
		}
		else
		{
			printf("4) I am the first parent with PID of %d and my second child's PID is %d\n", getpid(), a);
		}
	}

	return 0;
}
