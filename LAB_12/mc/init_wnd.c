#include "midc.h"

WINDOW *init_l_wnd(void)
{
    /*static*/ int rows = ROWS;
    /*static*/ int columns = COLS;
    /*static*/ int left = '|';
    /*static*/ int right = '|';
    /*static*/ int top = '-';
    /*static*/ int bottom = '-';
    /*static*/ int tlc, blc, trc, brc;
    tlc = blc = trc = brc = 0;

    /*static*/ WINDOW *l_wnd = NULL;
    l_wnd =  newwin(rows, columns, 2, 0);

    wborder(l_wnd, left = ACS_VLINE, right = ACS_VLINE, top = ACS_HLINE, bottom = ACS_HLINE, tlc = ACS_ULCORNER, trc = ACS_URCORNER, blc = ACS_LLCORNER, brc = ACS_LRCORNER);

    wrefresh(l_wnd);

    return l_wnd;
}

WINDOW *init_r_wnd(void)
{
    /*static*/ int rows = ROWS;
    /*static*/ int columns = COLS;
    /*static*/ int left = '|';
    /*static*/ int right = '|';
    /*static*/ int top = '-';
    /*static*/ int bottom = '-';
    /*static*/ int tlc, blc, trc, brc;
    tlc = blc = trc = brc = 0;

    /*static*/ WINDOW *r_wnd = NULL;
    r_wnd = newwin(rows, columns, 2, 101);

    wborder(r_wnd, left = ACS_VLINE, right = ACS_VLINE, top = ACS_HLINE, bottom = ACS_HLINE, tlc = ACS_ULCORNER, trc = ACS_URCORNER, blc = ACS_LLCORNER, brc = ACS_LRCORNER);

    wrefresh(r_wnd);

    return r_wnd;
}

WINDOW *init_menu(void)
{
    /*static*/ int rows = M_ROWS;
    /*static*/ int columns = M_COLS;
    /*static*/ int left = '|';
    /*static*/ int right = '|';
    /*static*/ int top = '-';
    /*static*/ int bottom = '-';
    /*static*/ int tlc, blc, trc, brc;
    tlc = blc = trc = brc = 0;

    /*static*/ WINDOW *menu_wnd = NULL;
    menu_wnd = newwin(rows, columns, 50, 0);

    wborder(menu_wnd, left = ACS_VLINE, right = ACS_VLINE, top = ACS_HLINE, bottom = ACS_HLINE, tlc = ACS_ULCORNER, trc = ACS_URCORNER, blc = ACS_LLCORNER, brc = ACS_LRCORNER);

    wrefresh(menu_wnd);

    return menu_wnd;
}

