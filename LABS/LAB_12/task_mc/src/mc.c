#include "midc.h"

#define SET_CURSOR_TO(self, y, x) do { \
                                    (self)->c_y = (y), (self)->c_x = (x); \
                                    wmove((self)->wnd[mc.side], (self)->c_y, (self)->c_x); \
                                } while(false);

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
