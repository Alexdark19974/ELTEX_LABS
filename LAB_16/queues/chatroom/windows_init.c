#include "chatroom_header.h"

WINDOW *init_user_list_wnd(void)
{
    int rows = 48;
    int columns = 30;
    int left = '|';
    int right = '|';
    int top = '-';
    int bottom = '-';
    int tlc, blc, trc, brc;
    tlc = blc = trc = brc = 0;

    WINDOW *user_list_wnd = newwin(rows, columns, 2, 151);

    if (NULL == user_list_wnd)
    {
        handle_error("newwin");
    }

    wborder(user_list_wnd, left = ACS_VLINE, right = ACS_VLINE, top = ACS_HLINE, bottom = ACS_HLINE, tlc = ACS_ULCORNER, trc = ACS_URCORNER, blc = ACS_LLCORNER, brc = ACS_LRCORNER);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);
    wbkgd(user_list_wnd, COLOR_PAIR(4));
    wattron(user_list_wnd, A_BOLD);
    mvwprintw(user_list_wnd, 1, 1, "*admin (PID: %d)", getpid());
    wrefresh(user_list_wnd);

    return user_list_wnd;

}

WINDOW *init_chit_chat_wnd(void)
{
    int rows = 45;
    int columns = 150;
    int left = '|';
    int right = '|';
    int top = '-';
    int bottom = '-';
    int tlc, blc, trc, brc; 
    tlc = blc = trc = brc = 0;

    WINDOW *chit_chat_wnd = newwin(rows, columns, 5, 0);

    if (NULL == chit_chat_wnd)
    {
        handle_error("newwin");
    }

    wborder(chit_chat_wnd, left = ACS_VLINE, right = ACS_VLINE, top = ACS_HLINE, bottom = ACS_HLINE, tlc = ACS_ULCORNER, trc = ACS_URCORNER, blc = ACS_LLCORNER, brc = ACS_LRCORNER);
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);
    wbkgd(chit_chat_wnd, COLOR_PAIR(3));
    wmove(chit_chat_wnd, 50, 0);
    wrefresh(chit_chat_wnd);

    return chit_chat_wnd;
}
WINDOW *init_tab_wnd(void)
{
    int rows = 3; 
    int columns = 150;
    int left = '|';
    int right = '|';
    int top = '-';
    int bottom = '-';
    int tlc, blc, trc, brc; 
    tlc = blc = trc = brc = 0;

    WINDOW *tab_wnd = NULL;
    tab_wnd = newwin(rows, columns, 2, 0);

    if (NULL == tab_wnd)
    {
        handle_error("newwin");
    }

    wborder(tab_wnd, left = ACS_VLINE, right = ACS_VLINE, top = ACS_HLINE, bottom = ACS_HLINE, tlc = ACS_ULCORNER, trc = ACS_URCORNER, blc = ACS_LLCORNER, brc = ACS_LRCORNER);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
    wbkgd(tab_wnd, COLOR_PAIR(2));
    wattron(tab_wnd, A_BOLD | A_STANDOUT);
    mvwprintw(tab_wnd, 1,1, "#CHATROOM");
    wmove(tab_wnd, 0, 0);
    wrefresh(tab_wnd);
    wattroff(tab_wnd, A_BOLD | A_BLINK);

    return tab_wnd;
}

