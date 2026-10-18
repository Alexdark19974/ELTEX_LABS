#include "midc.h"

void display_wnd(mc_s *self, win_t side, int offset)
{
    WINDOW *wnd = self->wnd[side];
    struct dirent **name_list = NULL;
    struct stat sb;
    int i = 0, y = 2;
    char str[STR_SIZE * 2] = {0};
    char *sp = NULL;

    memset(&sb, 0x0, sizeof(sb));

    mvwprintw(wnd, 1, 1, NAME_NUM);
    mvwprintw(wnd, 1, COLS / 4, NAME_STR);
    mvwprintw(wnd, 1, COLS / 3, SIZE_STR);
    mvwprintw(wnd, 1, (COLS / 3) + strlen(MODIFY_STR) + 1, MODIFY_TIME_STR);

    name_list = self->name_list[side];
    if (name_list) {
        for (i = offset, y = 2; i < self->n[side]; i++) {
            // if (!strcmp(name_list[i]->d_name, CUR_DIR_STR))
            //    continue;
            if (y >= LINES - 5)
                break;
            snprintf(str, sizeof(str), "%s/%s", self->path_way[side], name_list[i]->d_name);
            if (stat(str, &sb) == FAIL)
                continue;
            mvwprintw(wnd, y, 1, S_ISDIR(sb.st_mode) ? "/%s" : "%s", name_list[i]->d_name);
            if (!strcmp(name_list[i]->d_name, BACK_DIR_STR))
                mvwprintw(wnd, y, COLS / 3 - 1, UP_DIR_STR);
            else
               mvwprintw(wnd, y, COLS / 3, "%ld", sb.st_size);
            sp = ctime(&sb.st_mtime) + 4, sp[12] = '\0';
                mvwprintw(wnd, y, COLS / 3 + strlen(MODIFY_STR) + 1, "%s", sp);
            y++;
        }
    }

    wrefresh(wnd);
}

void display_line(mc_s *self, win_t side, int offset)
{
    WINDOW *wnd = self->menu_wnd[side];
    struct dirent **name_list = self->name_list[side];
    struct stat sb;
    char str[STR_SIZE * 2] = {0};
    int idx = self->c_y - 2 + offset;

    snprintf(str, sizeof(str), "%s/%s", self->path_way[side], name_list[self->c_y - 2]->d_name);
    if (stat(str, &sb) == FAIL)
        return;

    mvwprintw(wnd, 1, 1, "                                  ");
    if (!strcmp(name_list[self->c_y - 2]->d_name, BACK_DIR_STR))
        mvwprintw(wnd, 1, 1, UP_DIR_STR ",y=%d,idx=%d", self->c_y, idx);
    else
        mvwprintw(wnd, 1, 1, S_ISDIR(sb.st_mode) ? "F: /%s,y=%d,idx=%d" : "F:%s,y=%d,idx=%d", name_list[self->c_y - 2]->d_name, self->c_y, idx);

    wrefresh(wnd);
}