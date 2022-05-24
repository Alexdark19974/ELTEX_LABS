#include "midc.h"

int main(int argc, char **argv)
{
	extern char path_way[];
	extern char r_path_way[];
	extern int left_mode;
	extern int right_mode;

    getcwd(path_way, SIZE);
    getcwd(r_path_way, SIZE);

    initscr();
    cbreak();
    noecho();
    start_color();
    mousemask(BUTTON1_CLICKED, NULL);
    curs_set(TRUE);
    mvwprintw(stdscr, 1, 92, "Midnight Commander");
    wmove(stdscr, 0,0);
    refresh();

    WINDOW *l_wnd = init_l_wnd();
    WINDOW *r_wnd = init_r_wnd();
    WINDOW *menu_wnd = init_menu();

    mc(l_wnd, r_wnd, menu_wnd, left_mode, right_mode);

    endwin();

    exit(EXIT_SUCCESS);
}
