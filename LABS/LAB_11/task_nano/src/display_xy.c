#include "nano.h"

#ifdef DEBUG
void display_xy(nano_s *self)
{
    int prev_y, prev_x = 0;
    int max_y = 0, max_x = 0;
    char ch = self->lin_buf[self->c_y - 1][self->c_x - 1];
    char *ch_p = NULL;
    char str[STR_SIZ] = {0};

    strcpy(str, self->lin_buf[self->c_y - 1]);

    if ((ch_p = strchr(str, '\n'))) *ch_p = '\0';

    prev_y = self->c_y, prev_x = self->c_x;
    max_x = self->max_x;

    curs_set(FALSE);
    mvwprintw(self->menu_wnd, 1, self->m_x / 2 - 2, "                                  ");
    mvwprintw(self->menu_wnd, 1, self->m_x / 2 - 2, "str = %s, len = %zu", str, strlen(str));
    mvwprintw(self->menu_wnd, 2, self->m_x / 2 - 2, "                                  ");
    mvwprintw(self->menu_wnd, 2, self->m_x / 2 - 2, "ch = %#x, winch = %#x", ch, inch() & A_CHARTEXT);
    mvwprintw(self->menu_wnd, 3, self->m_x / 2 - 2, "                                  ");
    mvwprintw(self->menu_wnd, 3, self->m_x / 2 - 2, "y = %d, x = %d, mx = %d", prev_y, prev_x, max_x);
    self->c_y = prev_y, self->c_x = prev_x;
    move(prev_y, prev_x);
    wrefresh(self->menu_wnd);
    curs_set(TRUE);
}
#endif
