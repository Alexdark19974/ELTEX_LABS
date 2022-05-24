#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
 //	printf("Hello!\n");

	/* '\n' overflows the buffer, so the line will appear befor sleep ends*/

	printf("Hello!");

	/*No buffer overflow, so the line will appear only after sleep ends*/

	sleep(5);

	return 0;
}
