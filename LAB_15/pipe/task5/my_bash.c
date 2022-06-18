#include "bash-header.h"

void bash(void)
{
    int state = 0;
    int n = 0;
    char *buf = NULL;
    char *dir = NULL;
    char *tmp_ptr = NULL;
    char **args_list[MAX_ARGS_LIST] = {NULL};
    char *commands[MAX_COMMANDS] = {NULL};
    char *garbage_collector[MAX_GARBAGE_COLLECTOR] = {NULL};
    int gb_counter = 0;

    for (int i = 0; i < MAX_ARGS_LIST; i++)
    {
        args_list[i] = calloc(MAX_ARGS_LIST, sizeof(char **));
    }

    for (int i = 0; i < MAX_ARGS_LIST; i++)
    {
        for (int j = 0; j < MAX_ARGS_LIST; j++)
        {
            args_list[i][j] = calloc(MAX_ARGS_LIST, sizeof(char));
            garbage_collector[gb_counter] = args_list[i][j];
            gb_counter++;
        }
    }

    if (NULL == (buf = calloc(MAX_BUF, sizeof(char))))
    {
        handle_error("calloc");
    }

    if (NULL == (dir = calloc(MAX_DIR, sizeof(char))))
    {
        handle_error("calloc");
    }

    if (NULL == (tmp_ptr = calloc(MAX_BUF, sizeof(char))))
    {
        handle_error("calloc");
    }

    if (NULL == (getcwd(dir, MAX_DIR)))
    {
        handle_error("getcwd");
    }

    if (NULL == (strcat(dir, "$ ")))
    {
        handle_error("strcat");
    }

    if (-1 == (write(STDOUT_FILENO, "~", 1)))
    {
        handle_error_en(state, "write");
    }

    if (-1 == (write(STDOUT_FILENO, dir, MAX_DIR)))
    {
        handle_error_en(state, "write");
    }

	while ((n = read(STDIN_FILENO, buf, MAX_BUF)))
	{
        if (NULL != (strstr(buf, "|")))
        {
            int pipefd1[2];
            int pipe_count = 0;
            pid_t c_pid_1 = 0;
            pid_t c_pid_2 = 0;
            pid_t c_pid_3 = 0;

            if (-1 == (pipe(pipefd1)))
            {
                handle_error_en(state, "pipe");
            }

            pipe_count = string_parser(buf, args_list, commands, garbage_collector, &gb_counter);

            c_pid_1 = fork();

            if (-1 == c_pid_1)
            {
                handle_error_en(c_pid_1, "fork");
            }

            if (0 == c_pid_1)
            {
                state = dup2(pipefd1[1], STDOUT_FILENO);

                if (-1 == state)
                {
                    handle_error_en(state, "dup2");
                }

                close(pipefd1[1]);

                if (-1 == state)
                {
                    handle_error_en(state, "close");
                }

                close(pipefd1[0]);

                if (-1 == state)
                {
                    handle_error_en(state, "close");
                }

                state = execvp(commands[0], args_list[0]);

                if (-1 == state)
                {
                    handle_error_en(state, "execvp");
                }
            }
            else
            {
                int pipefd2[2];

                if (pipe_count == 2)
                {
                    pipe(pipefd2);
                }

                c_pid_2 = fork();

                if (0 == c_pid_2)
                {
                    state = dup2(pipefd1[0], STDIN_FILENO);

                    if (-1 == state)
                    {
                        handle_error_en(state, "dup2");
                    }

                    if (pipe_count == 2)
                    {
                        state = dup2(pipefd2[1], STDOUT_FILENO);

                        if (-1 == state)
                        {
                            handle_error_en(state, "dup2");
                        }

                        close(pipefd2[0]);

                        close(pipefd2[1]);
                    }

                    state  = close(pipefd1[0]);

                    if (-1 == state)
                    {
                        handle_error_en(state, "close");
                    }

                    state = close(pipefd1[1]);

                    if (-1 == state)
                    {
                        handle_error_en(state, "close");
                    }

                    state = execvp(commands[1], args_list[1]);

                    if (-1 == state)
                    {
                        handle_error_en(state, "execvp");
                    }
                }
                else
                {
                    if (pipe_count == 2)
                    {
                        c_pid_3 = fork();

                       if (0 == c_pid_3)
                        {

                            state = dup2(pipefd2[0], STDIN_FILENO);

                            if (-1 == state)
                            {
                                handle_error_en(state, "dup2");
                            }

                            close(pipefd2[0]);

                            close(pipefd2[1]);

                            state  = close(pipefd1[0]);

                            if (-1 == state)
                            {
                                handle_error_en(state, "close");
                            }

                            state = close(pipefd1[1]);

                            if (-1 == state)
                            {
                                handle_error_en(state, "close");
                            }

                            state = execvp(commands[2], args_list[2]);

                            if (-1 == state)
                            {
                                handle_error_en(state, "execvp");
                            }
                        }
                    }

                    close(pipefd1[0]);
                    close(pipefd1[1]);

                    if (pipe_count == 2)
                    {
                        close(pipefd2[0]);
                        close(pipefd2[1]);
                    }

                    wait_for_child(c_pid_1);
                    wait_for_child(c_pid_2);

                    if (pipe_count == 2)
                    {
                        wait_for_child(c_pid_3);
                    }

				    for (int i = 0; i < MAX_BUF; i++)
				    {
				    	buf[i] = '\0';
				    }

                }
            }
        }
        else
        {
            int c_pid = 0;

            state = string_parser(buf, args_list, commands, garbage_collector, &gb_counter);

		    c_pid = fork();

		    if (0 == c_pid)
		    {
                state = execvp(commands[0], args_list[0]);

                if (-1 == state)
                {
                    handle_error_en(state, "execvp");
                }

			    char *ptr = strstr(buf, "\n");
			    *ptr = '\0';
			    printf("%s: command not found\n", buf);

                for (int i = 0; buf[i] != '\0'; i++)
                {
                    buf[i] = '\0';
                }
		    }
		    else
		    {
		        c_pid = wait(&state);

			    if (state != 0)
			    {
                    handle_error_en(state, "wait");
			    }
			    else
			    {
				    for (int i = 0; i < MAX_BUF; i++)
				    {
				    	buf[i] = '\0';
				    }

				    for (int i = 0; buf[i] != '\0'; i++)
				    {
				    	tmp_ptr[i] = '\0';
				    }
			    }
		    }
        }

        state = write(STDOUT_FILENO, "~", 1);

        if (-1 == state)
        {
            handle_error_en(state, "write");
        }

		state = write(STDOUT_FILENO, dir, MAX_BUF);

        if (-1 == state)
        {
            handle_error_en(state, "write");
        }
    }

    if (0 == n)
    {
        for (int j = 0; j < MAX_GARBAGE_COLLECTOR; j++)
        {
            free(garbage_collector[j]);
            garbage_collector[j] = NULL;
        }

        for (int i = 0; i < MAX_ARGS_LIST; i++)
        {
            free(args_list[i]);
            args_list[i] = NULL;
         }

        free(dir);
        dir = NULL;

        free(buf);
        buf = NULL;

        free(tmp_ptr);
        tmp_ptr = NULL;
    }
}

