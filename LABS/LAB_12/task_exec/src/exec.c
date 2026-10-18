#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

int main(void)
{
	pid_t n = 0;
	int status = 0, ret = EXIT_SUCCESS;

	if ((n = fork())) {
		printf("Waiting for PID=%d...\n", n);
		(void)wait(&status);
		printf("status=%d\n", status);

		char *argv[10];
		int i = 0;

		if ((n = fork())) {
			printf("Waiting for PID=%d...\n", n);
			(void)wait(&status);
			printf("status=%d\n", status);
		} else {
			argv[i++] = "ping";
			argv[i++] = "192.168.0.1";
			argv[i++] = "-c";
			argv[i++] = "3";
			argv[i] = NULL;

			// v -argument vector, ending with NULL
			// execvp searches for the specified pgrom in directories from $PATH
			if ((ret = execvp(argv[0], argv)) < 0) {
				// this code should not work unless execlp fails
				fprintf(stderr, "execlp() failed: %s\n", strerror(errno));
			}
		}
	} else {
		// l - argument list, ending with NULL cast to char
		// execlp searches for the specified pgrom in directories from $PATH
		if ((ret = execlp("ls", "ls", "-la", ".", "../", (char *)NULL)) < 0) {
			// this code should not work unless execlp fails
			fprintf(stderr, "execlp() failed: %s\n", strerror(errno));
		}
	}

	return ret;
}
