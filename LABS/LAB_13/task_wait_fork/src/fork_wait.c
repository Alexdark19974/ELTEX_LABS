#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define NUM 3

int main(void)
{
	int i = 0/*, wstatus = 0*/;
	pid_t n = 0;

	printf("loop forking begins\n");	

	for (i = 0; i < NUM; i++) {
		printf("pre-fork i=%d\n", i);
		// every iteration spawns a child
		n = fork(); // another process with the same memory layout is spawned
		if (n) { // the parent inherits the child's PID
			printf("PID=%d, PPID=%d, CHPID=%d\n", getpid(), getppid(), n);
			wait(NULL/*&wstatus*/); // wait suspends execution of the process until the child's status has changed
		} else // every child eventually becomes a parent, n = 0 means it's a child process
			printf("PID=%d, PPID=%d\n", getpid(), getppid());
		// every iteration spawns a child
		printf("post-fork i=%d, PID=%d\n", i, getpid());
	}
	// child at i = 3 will never become a parent
	printf("loop forking is done (PID: %d, i=%d)\n", getpid(), i);

	return EXIT_SUCCESS;
}


/*
	iter 0:
	(parent)

	fork --> parent(1), wait
		 --> child (2)

	iter 1:
	(child, formerly parent)

	fork --> parent(2), wait
		 --> child (3)

	iter 2:
	(child, formerly parent)

	fork --> parent(3), wait
		 --> child (4)

	1)
		parent(PID:1) spawns a child (PID:2), waits,
		parent(PID:2) spawns a child (PID:3), waits,
		parent(PID:3) spawns a child (PID:4), waits,

	2)
		child (PID:4) leaves the loop
		parent (PID:3) is done waiting, leaves the loop

	3)
		parent (PID:2) gets to iter 2, spawns a child (PID:5), waits
		child(5) at iter 2 becomes the latest, leaving the loop.
		parent(PID:2) at iter 2 leaves the loop

	4)
		parent (PID:1) at iter 0 gets to iter 1, spawns a child (PID:6), and then waits
		child (PID:6) at iter 1 gets to iter 2, spawns a child (PID:7), and then waits
		child (PID:7) at iter 2 becomes the latest, leaving the loop
		parent (PID:6) at iter 2 leaves the loop

	5)
		parent (PID:1) at iter 1 gets to 2, spawns a child (PID:8), then waits
		child (PID:8) at iter 2 leaving the loop
		parent(PID:1) at iter 2 leaves the loop

	2^iter = total number of processes, iter is >= 1

	the one who is the latests terminates first, followed by the reverse loop behavior

	Exponential Growth Rule
	If fork() is called N times within a loop (and no child process exits or stops), the total number of processes created will be:
    Initial Process: 1 process.
    1st Iteration: The parent forks, resulting in 2 processes (2^1).
    2nd Iteration: Both existing processes fork, resulting in 4 processes (2^2).
    -th Iteration: Every existing process forks, resulting in (2^n) total processes.
*/
