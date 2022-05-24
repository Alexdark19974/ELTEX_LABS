#include "nano_h.h"

void input(char **file)
{
    int rows = 43;
    int columns = 80;
    int left = '|';
    int right = '|';
    int top = '-';
    int bottom = '-';
    int tlc, blc, trc, brc;
    tlc = blc = trc = brc = 0;
    int i = 0;
    int j = 0;
    int l = 0;
    int lock = 0;
    int ch = 0;
    int counter = 1;
    int x_max = 0;
    int y_max = 0;
    int cur_x = 0;
    int cur_y = 0;
    int fd = 0;
    int line_counter[BUFSIZ][1] = {0};
    char *data = malloc(BUFSIZ);
    char *buf = malloc(BUFSIZ);

    WINDOW *wnd = newwin(rows, columns, 1, 1);
    WINDOW *menuwnd = derwin(wnd, 4, 78, 38, 1);
    WINDOW *file_wnd = NULL;

   signal(SIGWINCH, sig_winch);

    DIR *dir_ptr = NULL;

    struct dirent *ctlg_ptr = NULL;

    char *file_str = NULL;

    getmaxyx(wnd, y_max, x_max);

    keypad(wnd, TRUE);

    mousemask(BUTTON1_CLICKED, NULL);

    wborder(wnd, left, right, top = ' ' | A_REVERSE, bottom, tlc = ' ' | A_REVERSE, trc = ' ' | A_REVERSE, blc = ACS_LLCORNER, brc = ACS_LRCORNER);

    wattron(wnd, A_REVERSE);

    mvwprintw(wnd, 0, 3, "GNU nano 4.8");

    mvwprintw(wnd, 0 , 40, "%s.c", file[0] + 2);

    wrefresh(wnd);

    wattroff(wnd, A_REVERSE);

    wborder(menuwnd, left = ' ', right = ' ', top = '-', bottom = ' ', tlc = '-', trc = '-', blc = ' ', brc = ' ');

    mvwprintw(menuwnd, 1, 1, "F1 - open file");

    mvwprintw(menuwnd, 3, 1, "F2 - save file");

    mvwprintw(menuwnd, 1, 18, "F3 - edit file");

    mvwprintw(menuwnd, 3, 18, "F4 - exit mnano");

    wrefresh(menuwnd);

    wmove(wnd, 1, 1);

    noecho();

    while (i < (BUFSIZ - 1))
    {
        while ((ch = wgetch(wnd)) != '\n')
        {
            if (lock == 0)
            {
                wattron(wnd, A_REVERSE);

                mvwprintw(wnd, 0, 70, "Modified");

                wmove(wnd, 1, 1);

                wrefresh(wnd);

                wattroff(wnd, A_REVERSE);

                lock = 1;
            }

            switch (ch)
            {
                case KEY_MOUSE:
                {
                    MEVENT event;

                    getmouse(&event);

                    if (event.y > 37)
                    {
                        break;
                    }
                    if (event.x > 78)
                    {
                        break;
                    }

                    wmove(wnd, event.y, event.x);

                    wrefresh(wnd);

                    break;
                }
                case KEY_F(1) :
                {
                    if (*(file + 1) == NULL)
                    {
                        mvwprintw(menuwnd, 2, 36, "No file found");

                        wmove(wnd, 1, 1);

                        wrefresh(menuwnd);

                        wrefresh(wnd);

                        continue;
                    }

                    dir_ptr = opendir("./food");


                   while ((ctlg_ptr = readdir(dir_ptr)) != NULL)
                    {
                        if (strcmp(*(file + 1), ctlg_ptr->d_name) == 0)
                        {
                            file_str = malloc(MALLOC_SIZE);

                            getcwd(file_str, MALLOC_SIZE);

                            strcat(file_str, "/food/");

                            strcat(file_str, *(file + 1));

                            if ((**(file + 2)) == 'r')
                            {
                                fd = open(file_str, O_RDONLY, 0);
                            }
                            else if ((**(file + 2)) == 'w')
                            {
                                fd = open(file_str, O_WRONLY, 0);
                                lseek(fd, 0, SEEK_END);
                            }

                            mvwprintw(menuwnd, 1, 36, "                ");

                            mvwprintw(menuwnd, 1, 36, "file opened");

                            wrefresh(menuwnd);

                            mvwprintw(menuwnd, 3, 36, "               ");

                            mvwprintw(menuwnd, 2, 36, "               ");

                            wrefresh(wnd);

                            wrefresh(menuwnd);

                            break;
                        }
                    }
                    file_wnd = newwin( y_max + 7, x_max + 10, 1, 84);

                    wborder(file_wnd, left = '|', right = '|', top = ' ' | A_REVERSE, bottom = '-', tlc = ' ' | A_REVERSE, trc = ' ' | A_REVERSE, blc = ACS_LLCORNER, brc = ACS_LRCORNER);

                    wattron(file_wnd, A_REVERSE);

                    mvwprintw(file_wnd, 0, 40, "%s", *(file + 1));

                    wattroff(file_wnd, A_REVERSE);

                    wmove(file_wnd, 1, 1);

                    if ((**(file + 2)) == 'r')
                    {
                        wmove(file_wnd, 1, 1);
                        int n = 0;

                        while ((n = read(fd, buf, BUFSIZ)) > 0)
                        {
                            ;
                        }

                        for (int length = 1, depth = 1, i = 0; buf[i] != '\0';)
                        {
                            while (buf[i] != '\n' && buf[i] != '\0')
                            {
                                mvwaddch(file_wnd, depth, length, buf[i]);
                                length++;
                                i++;
                                wrefresh(file_wnd);
                            }

                            i++;
                            length = 1;
                            depth++;
                        }

                        wrefresh(file_wnd);
                    }

                    wrefresh(file_wnd);

                    wmove(wnd, 1, 1);

                    wrefresh(wnd);

                    signal(SIGWINCH, sig_winch);

                   continue;
                }
                case KEY_F(2) :
                {
                    if (file_wnd == NULL && fd == 0 && dir_ptr == NULL)
                    {
                        mvwprintw(menuwnd, 2, 36, "No file found");

                        mvwprintw(menuwnd, 3, 36, "             ");

                        wmove(wnd, 1, 1);

                        wrefresh(menuwnd);

                        wrefresh(wnd);

                        continue;
                    }

                    if (fd != 0)
                    {
                        close(fd);
                        fd = 0;
                    }

                    if  (dir_ptr != NULL)
                    {
                        closedir(dir_ptr);
                        dir_ptr = NULL;
                    }

                    if (file_wnd != NULL)
                    {
                        wclear(file_wnd);

                        wrefresh(file_wnd);

                        delwin(file_wnd);

                        file_wnd = NULL;
                    }

                    if (file_str != NULL)
                    {
                        free(file_str);
                        file_str = NULL;
                        free(buf);
                        buf = NULL;
                    }

                  mvwprintw(menuwnd, 3, 36, "file closed");
                    wmove(wnd, 1, 1);

                    mvwprintw(menuwnd, 1, 36, "            ");

                    wrefresh(menuwnd);

                    wrefresh(wnd);
                    continue;
                }
                case KEY_F(3) :
                {
                    if (file_wnd == NULL)
                    {
                        mvwprintw(menuwnd, 2, 36, "No file found");

                        wmove(wnd, 1, 1);

                        wrefresh(menuwnd);

                        wrefresh(wnd);

                        continue;
                    }

                    if ((**(file + 2)) != 'r')
                    {

                        mvwprintw(menuwnd, 1, 36, "editing file");
                        wrefresh(menuwnd);
                        wrefresh(wnd);
                        refresh();
                    }

                    if ((**(file + 2)) == 'w')
                    {
                        char *string = malloc(100);

                        wmove(file_wnd, 1, 1);

                        wrefresh(file_wnd);

                        echo();

                        wgetnstr(file_wnd, string, 100);

                        int len = strlen(string) - 1;

                      write(fd, string, len);

                        wmove(file_wnd, 2, 1);

                        wclear(file_wnd);

                        wrefresh(file_wnd);

                        delwin(file_wnd);

                        file_wnd = NULL;

                        wrefresh(file_wnd);

                        mvwprintw(menuwnd, 1, 36, "changes saved");

                        wrefresh(menuwnd);

                        noecho();

                        wmove(wnd, 1, 1);

                        wrefresh(wnd);

                        free(string);
                    }
                    continue;
                }
                case KEY_F(4) :
                {
                    wattron(wnd, A_BLINK | A_BOLD);

                    curs_set(FALSE);

                    if (fd != 0)
                    {
                        close(fd);
                        fd = 0;
                    }

                               if  (dir_ptr != NULL)
                    {
                        closedir(dir_ptr);
                        dir_ptr = NULL;
                    }

                    if (file_wnd != NULL)
                    {
                        wclear(file_wnd);

                        wrefresh(file_wnd);

                        delwin(file_wnd);

                        file_wnd = NULL;
                    }

                    if (file_str != NULL)
                    {
                        free(file_str);
                        file_str = NULL;
                        free(buf);
                        buf = NULL;
                    }

                    free(data);
                    data = NULL;

                    mvwprintw(wnd, (y_max / 2) - 2, (x_max / 2) - 12, "press ANY key to exit");

                    wrefresh(wnd);

                    wgetch(wnd);

                    delwin(menuwnd);

                    delwin(wnd);

                    return;
                }
               case KEY_UP :
                {
                    getyx(wnd, cur_y, cur_x);

                    if (cur_y - 1 != 0)
                    {
                        wmove(wnd, cur_y - 1, cur_x);

                        wrefresh(wnd);
                    }
                    continue;
                }
                case KEY_DOWN :
                {
                    getyx(wnd, cur_y, cur_x);

                    if (cur_y + 1 != 38)
                    {
                        wmove(wnd, cur_y + 1, cur_x);

                        wrefresh(wnd);
                    }
                    continue;
                }
                case KEY_LEFT :
                {
                    getyx(wnd, cur_y, cur_x);

                    if (cur_x - 1 != 0)
                    {
                        wmove(wnd, cur_y, cur_x - 1);

                        wrefresh(wnd);
                    }
                    else
                    {
                        wmove(wnd, cur_y, x_max);

                        wrefresh(wnd);
                    }
                    continue;
                }
               case KEY_RIGHT :
                {
                    getyx(wnd, cur_y, cur_x);

                    if (cur_x + 1 != 79)
                    {

                        wmove(wnd, cur_y, cur_x + 1);

                        wrefresh(wnd);
                    }
                    continue;
                }
                case KEY_BACKSPACE :
                {
                    getyx(wnd, cur_y, cur_x);

                    if (cur_x - 1 > 0)
                    {
                        wmove(wnd, cur_y, cur_x - 1);

                        waddch(wnd, ' ');

                        wrefresh(wnd);

                        wmove(wnd, cur_y, cur_x - 1);

                        i--;

                        line_counter[j][l]--;
                    }
                    else
                    {
                        if ((cur_y - 1) == 0)
                        {
                            wmove(wnd, 1 , 1);
                            wrefresh(wnd);
                            counter = 1;
                            continue;
                        }

                        line_counter[j][l] = 0;

                        counter--;

                        j--;

                        wmove(wnd, cur_y - 1, cur_x + line_counter[j][l]);

                       wrefresh(wnd);
                    }
                    continue;
                }
                default:
                {

                    if (ch == '\t')
                    {
                        continue;
                    }


                    if (line_counter[j][l] == 78)
                    {
                        getyx(wnd, cur_y, cur_x);

                        if (cur_y + 1 == 38)
                        {
                            wmove(wnd, 1, 1);
                            counter = 1;
                            j = 0;
                            continue;
                        }
                        wmove(wnd, cur_y + 1, 1);
                        j++;
                        counter++;
                        continue;
                    }

                    data[i] = ch;
                    i++;
                    wechochar(wnd, ch);
                    line_counter[j][l] = line_counter[j][l] + 1;

                    break;
                }
            }
        }

       if (lock == 0)
        {
            wattron(wnd, A_REVERSE);

            mvwprintw(wnd, 0, 70, "Modified");

            wmove(wnd, 1, 1);

            wrefresh(wnd);

            wattroff(wnd, A_REVERSE);

            lock = 1;
        }

        if (counter == 37)
        {
            counter--;
            wrefresh(wnd);
            continue;
        }

        data[i] = ch;
        i++;
        data[i] = '\0';

        counter++;
        j++;

        wmove(wnd, counter, 1);

        wrefresh(wnd);

    }
}

