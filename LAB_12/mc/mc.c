#include "midc.h"

char path_way[SIZE] = {0};
char r_path_way[SIZE] = {0};
int left_mode = TRUE;
int right_mode = FALSE;

void mc(WINDOW *l_wnd, WINDOW *r_wnd, WINDOW *menu_wnd, int left_mode, int right_mode)
{
	keypad(l_wnd, TRUE);
	keypad(r_wnd, TRUE);

	int i, j;
	i = j = 0;
	int r_i, r_j;
	r_i = r_j = 0;
    char *str, *r_str;
	str = r_str = NULL;
	int ch = 0;
	int cur_y = 0;
	int cur_x = 0;
	int n = 0;
	int r_n = 0;
	int highlight = FALSE;
	struct dirent **name_list = NULL;
	struct dirent **r_name_list = NULL;
	char *data[MALLOC_SIZE] = {NULL};
	char *r_data[MALLOC_SIZE] = {NULL};

	for (int r = 0; r < MALLOC_SIZE; r++)
	{
		data[r] = calloc(MALLOC_SIZE, sizeof(char));
		r_data[r] = calloc(MALLOC_SIZE, sizeof(char));
	}

	n = scandir(path_way, &name_list, myselect, alphasort);
	r_n = scandir(r_path_way, &r_name_list, myselect, alphasort);

	if (n == ERROR || r_n == ERROR)
	{
		endwin();
		perror("scandir");
		exit(EXIT_FAILURE);
	}
	str = malloc(MALLOC_SIZE);
	r_str = malloc(MALLOC_SIZE);

	strcpy(str, path_way);
	strcpy(r_str, r_path_way);

	while ((i < n) && (name_list[i] != NULL))
	{
		if (strcmp(name_list[i]->d_name, "..") == 0)
		{
			if ((path_way[0] = '/') && (path_way[1] == '\0'))
			{
				i++;
				j++;
				strcpy(data[i - 1], "--END--");
				mvwprintw(l_wnd, 1, 1, "%s", data[i - 1]);
				continue;
			}
			strcpy(data[i], "../");
			mvwprintw(l_wnd, 1, 1, "%s", data[i]);
			i++;
			j++;
			continue;
		}
		else if ((strstr(name_list[i]->d_name, ".c") != NULL))
		{
			strcpy(data[i], name_list[i]->d_name);
	        mvwprintw(l_wnd, 1 + j , 1, "%s", data[i]);
	        j++;
	     	i++;
		}
		else if ((strstr(name_list[i]->d_name, ".swp") != NULL))
		{
			strcpy(data[i], name_list[i]->d_name);
	        mvwprintw(l_wnd, 1 + j , 1, "%s", data[i]);
	   	    j++;
	       	i++;
		}
		else if (strstr(name_list[i]->d_name, ".git") != NULL)
		{
			strcpy(data[i], "/");
			strcat(data[i], name_list[i]->d_name);
	        mvwprintw(l_wnd, 1 + j , 1, "%s", data[i]);
	        j++;
            i++;
		}
		else
		{
			strcat(str, "/");
			strcat(str, name_list[i]->d_name);

			if (isDirectory(str) != 0)
			{
				strcpy(data[i], "/");
				strcat(data[i], name_list[i]->d_name);
				mvwprintw(l_wnd, 1 + j , 1, "%s", data[i]);
	           	j++;
	           	i++;
			}
			else
			{
				strcpy(data[i], name_list[i]->d_name);
			    mvwprintw(l_wnd, 1 + j , 1, "%s", data[i]);
	       		j++;
	          	i++;
			}

			for (int l = strlen(str) - 1; str[l] != '/'; l--)
            {
                str[l] = '\0';

				if (str[l - 1] == '/')
				{
					str[l - 1] = '\0';
					break;
				}
           	}
		}
	}

	while ((r_i < r_n) && (r_name_list[r_i] != NULL))
    {
        if (strcmp(r_name_list[r_i]->d_name, "..") == 0)
        {
            if ((r_path_way[0] = '/') && (r_path_way[1] == '\0'))
            {
                r_i++;
                r_j++;
                strcpy(r_data[r_i - 1], "--END--");
                mvwprintw(r_wnd, 1, 1, "%s", r_data[r_i - 1]);
                continue;
            }
            strcpy(r_data[r_i], "../");
            mvwprintw(r_wnd, 1, 1, "%s", r_data[r_i]);
            r_i++;
            r_j++;
            continue;
        }
        else if ((strstr(r_name_list[r_i]->d_name, ".c") != NULL))
        {
            strcpy(r_data[r_i], r_name_list[r_i]->d_name);
            mvwprintw(r_wnd, 1 + r_j , 1, "%s", r_data[r_i]);
            r_j++;
            r_i++;
        }
        else if ((strstr(r_name_list[r_i]->d_name, ".swp") != NULL))
        {
            strcpy(r_data[r_i], r_name_list[r_i]->d_name);
            mvwprintw(r_wnd, 1 + r_j , 1, "%s", r_data[r_i]);
            r_j++;
            r_i++;
        }
        else if ((strstr(r_name_list[r_i]->d_name, ".git") != NULL))
        {
            strcpy(r_data[r_i], "/");
            strcat(r_data[r_i], r_name_list[r_i]->d_name);
            mvwprintw(r_wnd, 1 + r_j , 1, "%s", r_data[r_i]);
            r_j++;
           	r_i++;
        }
		else
        {
            strcat(r_str, "/");
        	strcat(r_str, r_name_list[r_i]->d_name);

            if (isDirectory(r_str) != 0)
            {
                strcpy(r_data[r_i], "/");
                strcat(r_data[r_i], r_name_list[r_i]->d_name);
                mvwprintw(r_wnd, 1 + r_j , 1, "%s", r_data[r_i]);
                r_j++;
                r_i++;
            }
            else
            {
            	strcpy(r_data[r_i], r_name_list[r_i]->d_name);
            	mvwprintw(r_wnd, 1 + r_j , 1, "%s", r_data[r_i]);
               	r_j++;
                r_i++;
            }

            for (int l = strlen(r_str) - 1; r_str[l] != '/'; l--)
            {
                r_str[l] = '\0';

                if (r_str[l - 1] == '/')
                {
                    r_str[l - 1] = '\0';
                    break;
                }
            }
        }
    }

	int el_max = i;
	int r_el_max = r_i;
	j = 0;
	r_j = 0;

	wrefresh(r_wnd);

	while (ch != EOF)
	{
		if (left_mode == TRUE)
		{
			if (path_way[0] == '/' && path_way[1] == '\0')
			{
				wmove(l_wnd, 2, 1);

				wattron(l_wnd, A_STANDOUT);

				j++;

				wprintw(l_wnd, "%s", data[j]);

				highlight = TRUE;
			}
			else
			{
				wmove(l_wnd, 1, 1);

				wattron(l_wnd, A_STANDOUT);

				wprintw(l_wnd, "%s", data[j]);

				highlight = TRUE;
			}
		}

		while ((left_mode == TRUE) && (ch = wgetch(l_wnd)) != EOF)
		{
			switch (ch)
			{
				case KEY_F(1):
				{
					for (int f = 0; f < MALLOC_SIZE; f++)
                    {
                    	if (r_data[f] != NULL)
                        {
                        	free(r_data[f]);
                            r_data[f] = NULL;
                        }

                        if (data[f] != NULL)
                        {
                        	free(data[f]);
                           	data[f] = NULL;
                        }
                    }

                    if (r_str != NULL)
                    {
                    	free(r_str);
                    }

                    if (str != NULL)
                    {
                    	free(str);
                    }

                   	delwin(l_wnd);
                   	l_wnd = NULL;
                   	delwin(r_wnd);
                   	r_wnd = NULL;
                   	delwin(menu_wnd);
                	endwin();
				    exit(EXIT_SUCCESS);
				}
				case KEY_RIGHT:
				{
					left_mode = FALSE;
					right_mode = TRUE;
					wattroff(l_wnd, A_STANDOUT);
					mvwprintw(l_wnd, 1, 1, "%s", data[0]);
					wrefresh(l_wnd);

					wattron(r_wnd, A_STANDOUT);

					mvwprintw(r_wnd, 1, 1, "%s", r_data[0]);
					wrefresh(r_wnd);

					highlight = TRUE;
					break;
				}
				case KEY_MOUSE:
				{
					MEVENT event;

					getmouse(&event);

					if (event.y >= ROWS)
					{
						break;
					}
					if (event.x == COLS)
					{
						break;
					}

					wmove(stdscr, 0, 0);

					wprintw(stdscr, "Current line is y: %d, x: %d", event.y, event.x);

					wrefresh(stdscr);

					wprintw(stdscr, "                                   ");

					wrefresh(stdscr);
					break;
				}
				case KEY_UP:
				{
					getyx(l_wnd, cur_y, cur_x);

					j--;

					if (j < 0)
					{
						if (path_way[0] == '/' && path_way[1] == '\0')
	    				{
							wattroff(l_wnd, A_STANDOUT);
					        wprintw(l_wnd, "%s", data[j + 1]);

							wmove(l_wnd, 2, 1);

							wattron(l_wnd, A_STANDOUT);
							highlight = TRUE;
							j = 1;
							wprintw(l_wnd, "%s", data[j]);
						}
						else
						{
							wmove(l_wnd, 1, 1);
							j = 0;
						}
						continue;
					}

		            if (j == 0)
		         	{
						if (path_way[0] == '/' && path_way[1] == '\0')
		                {
		                    wmove(l_wnd, 2, 1);
							j = 1;
							continue;
		                }

		                wattroff(l_wnd, A_STANDOUT);

	                    mvwprintw(l_wnd, cur_y, 1, "%s", data[j + 1]);

						wattron(l_wnd, A_STANDOUT);

		                wmove(l_wnd, 1,1);

		                wprintw(l_wnd, "%s", data[j]);

		                wattroff(l_wnd, A_STANDOUT);

		                wrefresh(l_wnd);

		                highlight = TRUE;
		                continue;
	                }

	   	            if (cur_y - 1 != 0)
	   	            {
						if (highlight == TRUE)
						{
							wattroff(l_wnd, A_STANDOUT);
							mvwprintw(l_wnd, cur_y, 1, "%s", data[j + 1]);

							highlight = FALSE;
						}

						wattron(l_wnd, A_STANDOUT);

	               		mvwprintw(l_wnd, cur_y - 1, 1, "%s", data[j]);

						wattroff(l_wnd, A_STANDOUT);

						wrefresh(l_wnd);

						highlight = TRUE;
	                }
					break;
				}
				case KEY_DOWN:
				{
					getyx(l_wnd, cur_y, cur_x);

					j++;

		            if (j > el_max - 1)
		            {
		                wmove(l_wnd, cur_y, 1);
						j--;
		                continue;
		            }

	            	if (cur_y + 1 != 47)
	                {
						if (highlight == TRUE)
						{
							wattroff(l_wnd, A_STANDOUT);
							mvwprintw(l_wnd, cur_y, 1, "%s", data[j - 1]);
							highlight = FALSE;
						}

						wattron(l_wnd, A_STANDOUT);

						wmove(l_wnd, cur_y + 1, 1);

						wprintw(l_wnd, "%s", data[j]);

		                wrefresh(l_wnd);

						highlight = TRUE;
		            }
					break;
				}
				case ENTER:
				{
					getyx(l_wnd, cur_y, cur_x);

					if (cur_y == 1)
		            {
						wclear(l_wnd);
		                wrefresh(l_wnd);

		                delwin(l_wnd);
						l_wnd = NULL;

		                wrefresh(stdscr);

						l_wnd = init_l_wnd();

						int res = strlen(path_way) - 1;

						for (; path_way[res] != '/'; res--)
						{
							path_way[res] = '\0';
						}

						if (res == 0)
						{
							path_way[res] = '/';
							res++;
							path_way[res] = '\0';
						}
						else
						{
							path_way[res] = '\0';
						}

						for (int f = 0; f < MALLOC_SIZE; f++)
						{
							free(data[f]);
							data[f] = NULL;
						}

						mc(l_wnd, r_wnd, menu_wnd, left_mode, right_mode);
	                }

					if (data[j][0] == '/' && data[j][1] != '.')
					{
						wclear(l_wnd);
		                wrefresh(l_wnd);

	                  	delwin(l_wnd);
						l_wnd = NULL;

	               		wrefresh(stdscr);

	              	   	l_wnd = init_l_wnd();

						if (str != NULL)
						{
							free(str);
							str = NULL;
						}


						if (path_way[0] == '/' && path_way[1] == '\0')
						{
							path_way[0] = '\0';
							strcat(path_way, data[j]);
						}
						else
						{
							strcat(path_way, data[j]);
						}

						for (int i = 0; i < MALLOC_SIZE; i++)
	               		{
	                       	if (data[i] != NULL)
	                       	{
	                       		free(data[i]);
	                          	data[i] = NULL;
	                       	}
	                   	}

	              	    mc(l_wnd, r_wnd, menu_wnd, left_mode, right_mode);
					}

					wmove(stdscr, 0,0);

					wprintw(stdscr, "Current line is y: %d, x: %d", cur_y, cur_x);

					wrefresh(stdscr);

					wmove(l_wnd, cur_y, cur_x);

					wrefresh(l_wnd);
					break;
			}
			default:
			{
				break;
			}
		}
	}

	if (right_mode == TRUE)
	{
		if (r_path_way[0] == '/' && r_path_way[1] == '\0')
	    {
	    	wattron(r_wnd, A_STANDOUT);

	       	r_j++;

	   	    mvwprintw(r_wnd, 2, 1, "%s", r_data[r_j]);

      		highlight = TRUE;
   		}
		else
		{
			wattron(r_wnd, A_STANDOUT);

			wmove(r_wnd, 1, 1);

            wattron(r_wnd, A_STANDOUT);

         	wprintw(r_wnd, "%s", r_data[r_j]);

			highlight = TRUE;
		}
	}

	while ((right_mode == TRUE) && (ch = wgetch(r_wnd)) != EOF)
	{
	    switch (ch)
	    {
			case KEY_F(1):
			{
				for (int f = 0; f < MALLOC_SIZE; f++)
	            {
	            	if (r_data[f] != NULL)
	                {
	                    free(r_data[f]);
	                    r_data[f] = NULL;
	                }

	                if (data[f] != NULL)
	                {
	                    free(data[f]);
	                   	data[f] = NULL;
	                }
	            }

				if (r_str != NULL)
				{
					free(r_str);
				}

				if (str != NULL)
				{
					free(str);
				}

				delwin(l_wnd);
				l_wnd = NULL;
				delwin(r_wnd);
				r_wnd = NULL;
				delwin(menu_wnd);
				endwin();
				exit(EXIT_SUCCESS);
			}
			case KEY_LEFT:
			{
				left_mode = TRUE;
				right_mode = FALSE;

				wattroff(r_wnd, A_STANDOUT);
	            mvwprintw(r_wnd, 1, 1, "%s", r_data[0]);
	           	wrefresh(r_wnd);

	            wattron(l_wnd, A_STANDOUT);

	            mvwprintw(l_wnd, 1, 1, "%s", data[0]);

	            wrefresh(l_wnd);

	            highlight = TRUE;
				break;
			}
	        case KEY_RIGHT:
	        {
	            left_mode = FALSE;
	            right_mode = TRUE;
	            break;
	       	}
	        case KEY_MOUSE:
	        {
	        	MEVENT event;

	            getmouse(&event);

	            if (event.y >= ROWS)
	            {
	            	break;
	            }

	            if (event.x == COLS)
	           	{
	            	break;
	            }

				wmove(stdscr, 0, 0);

	            wprintw(stdscr, "Current line is y: %d, x: %d", event.y, event.x);

	            wrefresh(stdscr);

                wprintw(stdscr, "                                   ");

                wrefresh(stdscr);
                break;
	        }
			case KEY_UP:
	        {
	            getyx(r_wnd, cur_y, cur_x);

	            r_j--;

	            if (r_j < 0)
	            {
	            	if (r_path_way[0] == '/' && r_path_way[1] == '\0')
	                {
	                	wattroff(r_wnd, A_STANDOUT);
	                    wprintw(r_wnd, "%s", r_data[r_j + 1]);

	                    wmove(r_wnd, 2, 1);

	                    wattron(r_wnd, A_STANDOUT);
	                    highlight = TRUE;
	                    r_j = 1;
	                    wprintw(r_wnd, "%s", r_data[r_j]);
	       	        }
	                else
	                {
	                	wmove(r_wnd, 1, 1);
	                    r_j = 0;
	                }
	                continue;
	            }

				if (r_j == 0)
	            {
	            	if (r_path_way[0] == '/' && r_path_way[1] == '\0')
	                {
	               		wmove(r_wnd, 2, 1);
	                    r_j = 1;
	                    continue;
	               	}

	                wattroff(r_wnd, A_STANDOUT);

	                mvwprintw(r_wnd, cur_y, 1, "%s", r_data[r_j + 1]);

	                wattron(r_wnd, A_STANDOUT);

	                wmove(r_wnd, 1,1);

	                wprintw(r_wnd, "%s", r_data[r_j]);

	                wattroff(r_wnd, A_STANDOUT);

                 	wrefresh(r_wnd);

	                highlight = TRUE;
	                continue;
	            }

				if (cur_y - 1 != 0)
	            {
	            	if (highlight == TRUE)
                    {
	                	wattroff(r_wnd, A_STANDOUT);
	                    mvwprintw(r_wnd, cur_y, 1, "%s", r_data[r_j + 1]);
	                    highlight = FALSE;
	                }

	                wattron(r_wnd, A_STANDOUT);

	                mvwprintw(r_wnd, cur_y - 1, 1, "%s", r_data[r_j]);

	                wattroff(r_wnd, A_STANDOUT);

	                wrefresh(r_wnd);

	                highlight = TRUE;
	            }

	        	break;
	        }
			case KEY_DOWN:
	        {
	                getyx(r_wnd, cur_y, cur_x);

	                r_j++;

	                if (r_j > r_el_max - 1)
	                {
	                	wmove(r_wnd, cur_y, 1);
	                    r_j--;
	                    continue;
	                }

	                if (cur_y + 1 != 47)
	                {
	                    if (highlight == TRUE)
	                    {
	                    	wattroff(r_wnd, A_STANDOUT);
                        	mvwprintw(r_wnd, cur_y, 1, "%s", r_data[r_j - 1]);
                        	highlight = FALSE;
                    	}

                    	wattron(r_wnd, A_STANDOUT);

                    	wmove(r_wnd, cur_y + 1, 1);

	                    wprintw(r_wnd, "%s", r_data[r_j]);

    	                wrefresh(r_wnd);

        	            highlight = TRUE;
	                }

	            	break;
	            }
				case ENTER:
	            {
	                getyx(r_wnd, cur_y, cur_x);

                    if (cur_y == 1)
                    {
  	                    wclear(r_wnd);
	                    wrefresh(r_wnd);
	                    delwin(r_wnd);
						r_wnd = NULL;

	                    wrefresh(stdscr);

	                    r_wnd = init_r_wnd();

	                    int res = strlen(r_path_way) - 1;

	                    for (; r_path_way[res] != '/'; res--)
	                    {
	                        r_path_way[res] = '\0';
	                    }

	                    if (res == 0)
	                    {
	                        r_path_way[res] = '/';
	                        res++;
	                        r_path_way[res] = '\0';
	                    }
	                    else
	                    {
	                        r_path_way[res] = '\0';
	                    }

	                    for (int f = 0; f < MALLOC_SIZE; f++)
	                    {
							if (r_data[f] != NULL)
							{
	                           	free(r_data[f]);
                            	r_data[f] = NULL;
							}

							if (data[f] != NULL)
							{
								free(data[f]);
								data[f] = NULL;
							}
                        }

                        mc(l_wnd, r_wnd, menu_wnd, left_mode, right_mode);
                    }

					if (r_data[r_j][0] == '/' && r_data[r_j][1] != '.')
                    {
                        wclear(r_wnd);
                        wrefresh(r_wnd);

  	                    delwin(r_wnd);
						r_wnd = NULL;

                        wrefresh(stdscr);

                        r_wnd = init_r_wnd();

                        if (r_str != NULL)
                        {
                            free(r_str);
                            r_str = NULL;
                        }

						if (r_path_way[0] == '/' && r_path_way[1] == '\0')
						{
							r_path_way[0] = '\0';
							strcat(r_path_way, r_data[r_j]);
						}
						else
						{
                        	strcat(r_path_way, r_data[r_j]);
						}

                        for (int r_i = 0; r_i < MALLOC_SIZE; r_i++)
                        {
                            if (r_data[r_i] != NULL)
                            {
                                free(r_data[r_i]);
                                r_data[r_i] = NULL;
                            }
                        }

                        mc(l_wnd, r_wnd, menu_wnd, left_mode, right_mode);
				    }

                    wmove(stdscr, 0,0);

                    wprintw(stdscr, "Current line is y: %d, x: %d", cur_y, cur_x);

                    wrefresh(stdscr);
                    wmove(r_wnd, cur_y, cur_x);

                    wrefresh(r_wnd);

                    break;
 	           	}
    	        default:
        	    {
            	    break;
            	}
			}
		}
	}
}
