#include "nano.h"

void redraw(nano_s *self, WND_E wnd_type)
{
    int left = '|', right = '|', top = '-', bottom = '-';
    int tlc = 0, blc = 0, trc = 0, brc = 0;

    switch (wnd_type) {
        case STDSCR: {
            border(left, right,
                    top = ' ' | A_REVERSE, bottom,
                    tlc = ' ' | A_REVERSE, trc = ' ' | A_REVERSE,
                    blc = ACS_LLCORNER, brc = ACS_LRCORNER);
            refresh();
        }
        break;
        case MENUSCR:
            switch (self->ch) {
                case KEY_F(2):
                    {
                        FILE *fp = NULL;
                        char *ch_p = NULL;
                        int i = 0;
                        char file_name[FILE_NAME] = {0}, file_path[PATH_MAX] = {0};

                        wclear(self->menu_wnd);
                        wborder(self->menu_wnd,
                            left = ' ', right = ' ', top = '-', bottom = ' ', tlc = '-', trc = '-', blc = ' ', brc = ' ');
                        
                        echo();

                        mvwprintw(self->menu_wnd, 1, 1,     "Write to file: ");
                        wgetnstr(self->menu_wnd, file_name, sizeof(file_name) - 1);

                        if ((ch_p = strchr(file_name, NEWLINE)))
                            *ch_p = '\0';

                        wclear(self->menu_wnd);
                        wborder(self->menu_wnd,
                            left = ' ', right = ' ', top = '-', bottom = ' ', tlc = '-', trc = '-', blc = ' ', brc = ' ');

                        mvwprintw(self->menu_wnd, 1, 1,     "Path to file: ");
                        wgetnstr(self->menu_wnd, file_path, sizeof(file_path) - 1);

                        if (IS_NEWLINE(*file_path))
                            if (!getcwd(file_path, sizeof(file_path))) {
                                endwin();
                                exit(EXIT_FAILURE);
                            }

                        if ((ch_p = strchr(file_path, NEWLINE)))
                            *ch_p = '\0';

                        strcat(file_path, "/");
                        strcat(file_path, file_name);

                        if (!(fp = fopen(file_path, "w"))) {
                            snprintf(self->err_buf, ERR_BUFSIZ, "[%s:%d] error: fopen '%s' failed! (%s)", __func__, __LINE__, file_path, strerror(errno));
                            return;
                        }

                        for (i = 0; *self->lin_buf[i]; i++)
                            fputs(self->lin_buf[i], fp);
                        fclose(fp);
                    }
                    break;
                default:
                    wborder(self->menu_wnd,
                        left = ' ', right = ' ', top = '-', bottom = ' ', tlc = '-', trc = '-', blc = ' ', brc = ' ');
                    mvwprintw(self->menu_wnd, 1, 1,     "F1 - open file");
                    mvwprintw(self->menu_wnd, 3, 1,     "F2 - save file");
                    mvwprintw(self->menu_wnd, 1, 18,    "F3 - edit file");
                    mvwprintw(self->menu_wnd, 3, 18,    "F4 - exit nano");
                    break;
            }
            self->ch = 0;
            wrefresh(self->menu_wnd);
            break;
        default:
            break;
    }
}
