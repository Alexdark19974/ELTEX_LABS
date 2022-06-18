#include "bash-header.h"

int string_parser(char *line_to_be_parsed, char ***parsed_args_list, char **parsed_commands, char **garbage_collector, int *gb_counter)
{
    char *delim = " ";
    char *strstr_ptr = NULL;
    int **gb_counter_ptr = &gb_counter;
    int patterns_counter = 1;
    int pipe_count = 0;
    static char *patterns[MAX_PATTERNS] =
    {
        "ls",
        "grep",
        "nl",
        "pwd",
        "tail",
        "head",
        "mc",
        "pstree",
        "clear",
        "exit",
        "ps",
        NULL,
    };

    parsed_args_list[0][0] = strtok(line_to_be_parsed, delim);

    for (int i = 0; i < MAX_ARGS_LIST; i++)
    {
        if (NULL != (strstr(parsed_args_list[0][0], patterns[i])))
        {
            if (NULL != (strstr_ptr = strstr(parsed_args_list[0][0], "\n")))
            {
                *strstr_ptr = '\0';
                parsed_commands[0] = strdup(patterns[i]);
                garbage_collector[**gb_counter_ptr] = parsed_commands[0];
                **gb_counter_ptr = **gb_counter_ptr + 1;
                parsed_args_list[0][1] = NULL;
                return 0;
            }

            parsed_commands[0] = strdup(patterns[i]);
            garbage_collector[**gb_counter_ptr] = parsed_commands[0];
            **gb_counter_ptr = **gb_counter_ptr + 1;
            break;
        }
        if (patterns[i + 1] == NULL)
        {
            fprintf(stderr, "such program command is not on the list. Plead, add manually via editing string_parcer.c, var *patterns[MAX_PATTERNS]\n");
            exit(EXIT_FAILURE);
        }
    }

    int j = 1;

    for (int i = 0; i < MAX_ARGS_LIST; i++)
    {
        for (; j < MAX_ARGS_LIST; j++)
        {
            parsed_args_list[i][j] = strtok(NULL, delim);

            if (isspace(parsed_args_list[i][j][0]))
            {
                parsed_args_list[i][j][0] = '\0';
                parsed_args_list[i][j] = NULL;
                return 0;
            }

            for (int k = 0; patterns[k] != NULL; k++)
            {
                if (NULL != strstr(parsed_args_list[i][j], patterns[k]))
                {
                    parsed_commands[patterns_counter] = strdup(patterns[k]);
                    garbage_collector[**gb_counter_ptr] = parsed_commands[patterns_counter];
                    **gb_counter_ptr = **gb_counter_ptr + 1;
                    patterns_counter++;
                    break;
                }
                
                if (patterns[k + 1] == NULL)
                {
                    fprintf(stderr, "such program command is not on the list. Plead, add manually via editing string_parcer.c, var *patterns[MAX_PATTERNS]\n");
                    exit(EXIT_FAILURE);
                }
            }

            if (NULL != (strstr_ptr = strstr(parsed_args_list[i][j], "\n")))
            {
                *strstr_ptr = '\0';
                parsed_args_list[i][j + 1] = NULL;
                return pipe_count > 1 ? pipe_count : 0;
            }

            if ((NULL != strstr(parsed_args_list[i][j], "|")))
            {
                pipe_count++;
                parsed_args_list[i][j] = NULL;
                break;
            }
        }

        j = 0;
    }

    return 0;
}
