#include "midc.h"

#define SET_CURSOR_TO(self, y, x) do { \
                                    (self)->c_y = (y), (self)->c_x = (x); \
                                    wmove((self)->wnd[mc.side], (self)->c_y, (self)->c_x); \
                                } while(false);

int main(int argc, char **argv)
{
    struct sigaction act = {0};

    (void)argc, (void)argv;

    act.sa_sigaction = &s_handler;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGSEGV, &act, NULL);

    return start_mc();
}

int init_mc(mc_s *self)
{
    int i = 0;

    memset(self, 0x0, sizeof(mc_s));

    for (i = 0; i < WIN_MAX; i++) {
        if (!getcwd(self->path_way[i], sizeof(self->path_way[i])))
            return FAIL;
        self->wnd[i] = init_wnd(self, (win_t)i);
        self->menu_wnd[i] = init_menu_wnd(self, (win_t)i);
        self->name_list[i] = NULL;
        keypad(self->wnd[i], TRUE);
        memset(self->data[i], 0x0, sizeof(self->data[i]));
        self->n[i] = 0;
    }

    self->side = LEFT;
    self->display_wnd = display_wnd;
    self->display_line = display_line;
    self->get_curdir_data = get_curdir_data;
    self->free_curdir_data = free_curdir_data;
    self->init_wnd = init_wnd;
    self->init_menu_wnd = init_menu_wnd;

    return OK;
}

void deinit_mc(mc_s *self)
{
    int i = 0;

    for (i = 0; i < WIN_MAX; i++) {
        delwin(self->wnd[i]);
        delwin(self->menu_wnd[i]);

        while(self->n[i]--)
            free(self->name_list[i][self->n[i]]);
        free(self->name_list[i]);
    }
    endwin();
}

WINDOW *init_wnd(mc_s *self, win_t side)
{
    WINDOW *wnd = NULL;

    if (!self->wnd[side])
        wnd = side == LEFT ? newwin(LINES - 4, COLS / 2, 0, 0) : newwin(LINES - 4, COLS / 2, 0, COLS / 2);
    else
        wnd = self->wnd[side];

    wborder(wnd, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    wrefresh(wnd);

    return wnd;
}

WINDOW *init_menu_wnd(mc_s *self, win_t side)
{
    WINDOW *wnd = NULL;

    if (!self->menu_wnd[side])
        wnd = side == LEFT ? newwin(4, COLS / 2, LINES - 4, 0) : newwin(4, COLS / 2, LINES - 4, COLS / 2);
    else
        wnd = self->menu_wnd[side];

    wborder(wnd, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    wrefresh(wnd);

    return wnd;
}

int start_mc(void)
{
    int idx = 0, offset = 0;
    char *path = NULL;
    WINDOW *wnd = NULL;
    struct dirent **name_list = NULL;
    bool loop = true;
    win_t side = LEFT;
    mc_s mc;
    char str[STR_SIZE * 2] = {0};
    struct stat sb;

    initscr();
    cbreak();
    noecho();
    start_color();
    mousemask(BUTTON1_CLICKED, NULL);
    curs_set(TRUE);

    if (FAIL == init_mc(&mc))
        goto err;
    if (FAIL == mc.get_curdir_data(&mc, LEFT))
        goto err;
    if (FAIL == mc.get_curdir_data(&mc, RIGHT))
        goto err;

    mc.display_wnd(&mc, LEFT, offset);
    mc.display_wnd(&mc, RIGHT, offset);

    SET_CURSOR_TO(&mc, 2, 1);

    mc.display_line(&mc, LEFT, false);
    mc.display_line(&mc, RIGHT, false);

    while (loop) {
        side = mc.side;
        wnd = mc.wnd[side];
        path = mc.path_way[side];
        name_list = mc.name_list[side];
        idx = mc.c_y - 2 + offset;
        mvwchgat(wnd, mc.c_y, mc.c_x, COLS / 2 - 2, A_STANDOUT, 0, NULL);
        mc.display_line(&mc, side, offset);

        switch ((mc.ch = wgetch(wnd))) {
            case EOF:
                loop = false;
                break;
            case KEY_UP:
                if (mc.c_y > 2) {
                    mvwchgat(wnd, mc.c_y, mc.c_x, COLS / 2 - 2, A_NORMAL, 0, NULL);
                    wmove(wnd, --mc.c_y, mc.c_x);
                } else if (offset) {
                    wclear(wnd);
                    mc.init_wnd(&mc, side);
                    mc.display_wnd(&mc, side, --offset);
                }
                break;
            case KEY_DOWN:
                if ((idx + 1 < mc.n[side]) && (mc.c_y + 1 < LINES - 5)) { // idx + 1 is 
                    mvwchgat(wnd, mc.c_y, mc.c_x, COLS / 2 - 2, A_NORMAL, 0, NULL);
                    wmove(wnd, ++mc.c_y, mc.c_x);
                } else if ((idx + 1 < mc.n[side]) && (mc.c_y + 1 >= LINES - 5)) {
                    wclear(wnd);
                    mc.init_wnd(&mc, side);
                    mc.display_wnd(&mc, side, ++offset);
                }
                break;
            case KEY_RIGHT:
                if (mc.side != RIGHT) {
                    mvwchgat(wnd, mc.c_y, mc.c_x, COLS / 2 - 2, A_NORMAL, 0, NULL);
                    wrefresh(wnd);
                    mc.side = RIGHT;
                }
                break;
            case KEY_LEFT:
                if (mc.side != LEFT) {
                    mvwchgat(wnd, mc.c_y, mc.c_x, COLS / 2 - 2, A_NORMAL, 0, NULL);
                    wrefresh(wnd);
                    mc.side = LEFT;
                }
                break;
            case ENTER:
                memset(&sb, 0x0, sizeof(sb));
                snprintf(str, sizeof(str), "%s/%s", path, name_list[idx]->d_name);
                if (FAIL == stat(str, &sb))
                    break;
                if (S_ISDIR(sb.st_mode)) {
                    wclear(wnd);
                    if (FAIL == chdir(str))
                        goto err;
                    if (!getcwd(path, STR_SIZE))
                        goto err;
                    free_curdir_data(&mc, side);
                    mc.init_wnd(&mc, side);
                    if (FAIL == mc.get_curdir_data(&mc, side))
                        goto err;
                    offset = 0;
                    mc.display_wnd(&mc, side, offset);
                    SET_CURSOR_TO(&mc, 2, 1);
                }
                break;
            default:
                break;
        }
    }

err:
    deinit_mc(&mc);

    return EXIT_SUCCESS;
}

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

int get_curdir_data(mc_s *self, win_t side)
{
    return (self->n[side] = scandir(self->path_way[side], &self->name_list[side], NULL, alphasort));
}

void free_curdir_data(mc_s *self, win_t side)
{
    if (self->name_list[side]) {
        while (self->n[side]--) // evaluate, increment inside loop body
            free(self->name_list[side][self->n[side]]);
        free(self->name_list[side]);
        self->name_list[side] = NULL;
        self->n[side] = 0;
    }
}

void s_handler(int sig_no, siginfo_t *sig_info, void *sig_context)
{
    (void)sig_context;
    (void)sig_info;

    switch(sig_no) {
        case SIGSEGV:
            endwin();
            fprintf(stderr, "[%s:%d] programm exited with SIGSEGV\n", __func__, __LINE__);
            exit(EXIT_FAILURE);
            break;
        default:
            break;
    }
}
