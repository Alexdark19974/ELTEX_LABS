#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>

void bash(void);

int main (int argc, char *argv[])
{
	bash();

	return 0;
}

void bash(void)
{
	char *buf = calloc(BUFSIZ, sizeof(char));
	char *dir = calloc(BUFSIZ, sizeof(char));
	char *tmp_ptr = calloc(100, sizeof(char));
	int n = 0;
	int status = 0;
	pid_t c = 0;

	getcwd(dir, BUFSIZ);
    strcat(dir, "$ ");

	write(STDOUT_FILENO, dir, BUFSIZ);

	while ((n = read(STDIN_FILENO, buf, BUFSIZ)) > 0)
	{
		c = fork();

		if (c == 0)
		{
			if (strstr(buf, "ls") != NULL)
			{
				if ((tmp_ptr = strstr(buf, "-")) != NULL)
				{
					while (*(++tmp_ptr) != '\0')
					{
						switch (*tmp_ptr)
						{
							case  'l':
							{

								if (*(tmp_ptr + 1) == 'a')
								{
									execlp("ls", "ls", "-al", NULL);

                                    printf("error value: %d\n", errno);

                                    perror("Error message");

                                    exit(EXIT_FAILURE);
								}
								else
								{
									execlp("ls", "ls", "-l", NULL);

				                    printf("error value: %d\n", errno);

			                    	perror("Error message");

            			        	exit(EXIT_FAILURE);
								}

								break;
							}
							case 'a':
							{
								if (*(tmp_ptr + 1) == 'l')
								{
									execlp("ls", "ls", "-al", NULL);

                                    printf("error value: %d\n", errno);

                                    perror("Error message");

                                    exit(EXIT_FAILURE);
								}
								else
								{
									execlp("ls", "ls", "-a", NULL);

                                	printf("error value: %d\n", errno);

                                	perror("Error message");

                                	exit(EXIT_FAILURE);
								}

								break;
							}
							default:
							{
								printf("unknown option");
								break;
							}
						}
					}
				}
				else
				{
					execlp("ls", "ls", NULL);

                    printf("error value: %d\n", errno);

                    perror("Error message");

                    exit(EXIT_FAILURE);
				}
			}

			if (strstr(buf, "pwd") != NULL)
			{
				execlp("pwd", "pwd", NULL);

				printf("error value: %d\n", errno);

                perror("Error message");

                exit(EXIT_FAILURE);
			}

			if (strstr(buf, "clear") != NULL)
            {
                execlp("clear", "clear", NULL);

                printf("error value: %d\n", errno);

                perror("Error message");

                exit(EXIT_FAILURE);
            }

			if (strstr(buf, "nano") != NULL)
            {
                execlp("nano", "nano", NULL);

                printf("error value: %d\n", errno);

                perror("Error message");

                exit(EXIT_FAILURE);
            }

			if (strstr(buf, "mc") != NULL)
            {
                execlp("mc", "mc", NULL);

                printf("error value: %d\n", errno);

                perror("Error message");

                exit(EXIT_FAILURE);
            }

			if (strstr(buf, "exit") != NULL)
            {
                execlp("exit", "exit", NULL);

                printf("error value: %d\n", errno);

                perror("Error message");

                exit(EXIT_FAILURE);
            }

			if (strstr(buf, "ps") != NULL)
            {
				if (strstr(buf, "pstree") != NULL)
            	{
                	execlp("pstree", "pastree", NULL);

                	printf("error value: %d\n", errno);

	                perror("Error message");

    	            exit(EXIT_FAILURE);
        	    }
				else
				{
                	execlp("ps", "ps", NULL);

                	printf("error value: %d\n", errno);

                	perror("Error message");

                	exit(EXIT_FAILURE);
				}
            }

			char *ptr = strstr(buf, "\n");
			*ptr = '\0';
			printf("%s: command not found\n", buf);
		}
		else
		{
			 wait(&status);

			if (status != 0)
			{
				printf("error value: %d\n", errno);

				perror("Error message");

				exit(EXIT_FAILURE);
			}
			else
			{
//				printf("PID: %d\n", c);

				for (int i = 0; buf[i] != '\0'; i++)
				{
					buf[i] = '\0';
				}

				for (int i = 0; buf[i] != '\0'; i++)
				{
					tmp_ptr[i] = '\0';
				}
			}
		}

		write(STDOUT_FILENO, dir, BUFSIZ);
	}

}
