#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#define NUM 3

int main(void)
{
	int i = 0/*, wstatus = 0*/;
	pid_t n = 0;

	printf("loop forking begins\n");	

	for (i = 0; i < NUM; i++) {
		printf("pre-fork i=%d, PID:%d\n", i, getpid());
		// every iteration spawns a child
		n = fork(); // another process with the same memory layout is spawned
		if (n) { // the parent inherits the child's PID
			printf("PID=%d, PPID=%d, CHPID=%d\n", getpid(), getppid(), n);
			// now do ps -a to see zombies marked by 'Z' and 'defunct'
			sleep(10);
			// wait(NULL/*&wstatus*/); // wait suspends execution of the process until the child's status has changed
		} else // child whose parent process terminates before the child and who continues execution is orphaned, subsequently owned by PID 1 (init)
			printf("PID=%d, PPID=%d(%s)\n", getpid(), getppid(), getppid() == 1 ? "(orphaned)" : ("known"));
		// every iteration spawns a child
		printf("post-fork i=%d, PID=%d\n", i, getpid());
	}
	// child at i = 3 will never become a parent
	printf("loop forking is done (PID: %d, i=%d)\n", getpid(), i);

	// we wait no one. Even if it results in zombification en masse
	// if the child process terminates and the parent process doesn't read its status via wait(), it becomes a zombie.
	// it's a waste of a PID even if it wastes no CPU resources/processing
	// a zombie process will stay this way until its parent terminates, subsequently being reaped / orphaned by PID 1 (init)
	return EXIT_SUCCESS;
}

/*
	Feature 		|	Zombie Process 		|	Orphan Process
	State 			|	Terminated (dead) 	|	Running (alive)
	Parent			| 	Alive, but inactive	| 	Dead (terminated)
	Process Table  	| 	Occupies entry		|	Adopted by init (PID 1)
	Resource Usage	|	Only PID slot		|	Consumes RAM/CPU
	Resolution		|	Parent calls wait()	|	init reaps upon exit

	Key Takeaways
    1) Zombies are "defunct" or "zombie" (state Z) in ps output;

    2) Orphans are usually benign; the OS cleans them up efficiently;

    3) Too many zombies can fill the process table, preventing new processes from starting.

    4) Parent process must read its child's exit status
*/