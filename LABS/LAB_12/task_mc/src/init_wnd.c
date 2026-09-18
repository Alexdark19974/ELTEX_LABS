#include "midc.h"

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
