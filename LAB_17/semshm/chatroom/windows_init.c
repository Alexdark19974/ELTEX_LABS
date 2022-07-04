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
    int columns = 106;
    int left = '|';
    int right = '|';
    int top = '-';
    int bottom = '-';
    int tlc, blc, trc, brc; 
    tlc = blc = trc = brc = 0;
    int tab_beg_y = 0;
    int tab_beg_x = 0;

    WINDOW *tab_wnd = NULL;
    tab_wnd = newwin(rows, columns, 2, 0);

    wborder(tab_wnd, left = ACS_VLINE, right = ACS_VLINE, top = ACS_HLINE, bottom = ACS_HLINE, tlc = ACS_ULCORNER, trc = ACS_URCORNER, blc = ACS_LLCORNER, brc = ACS_LRCORNER);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
    wbkgd(tab_wnd, COLOR_PAIR(2));
    wattron(tab_wnd, A_BOLD | A_STANDOUT);
    mvwprintw(tab_wnd, 1,1, "#CHATROOM");
    wmove(tab_wnd, 0, 0);
    wrefresh(tab_wnd);
    wattroff(tab_wnd, A_BOLD | A_BLINK);

    getbegyx(tab_wnd, tab_beg_y, tab_beg_x);

    tab_beg_y--; 
    tab_beg_x+=12;

    wattron(tab_wnd, A_BOLD);
    mvwprintw(tab_wnd, tab_beg_y, tab_beg_x, "When chatting with users, type in 'q' and press ENTER to quit.");
    wrefresh(tab_wnd); 
    wattroff(tab_wnd, A_BOLD);

    return tab_wnd;
}

WINDOW *init_update_wnd(void)
{
    int rows = 3; 
    int columns = 44;
    int left = '|';
    int right = '|';
    int top = '-';
    int bottom = '-';
    int tlc, blc, trc, brc; 
    tlc = blc = trc = brc = 0;

    WINDOW *update_wnd = NULL;
    update_wnd = newwin(rows, columns, 2, 106);
    wborder(update_wnd, left = ACS_VLINE, right = ACS_VLINE, top = ACS_HLINE, bottom = ACS_HLINE, tlc = ACS_ULCORNER, trc = ACS_URCORNER, blc = ACS_LLCORNER, brc = ACS_LRCORNER);
    init_pair(5, COLOR_BLACK, COLOR_RED);
    wbkgd(update_wnd, COLOR_PAIR(5));
    wattron(update_wnd, A_BOLD | A_BLINK);
    wrefresh(update_wnd);

    return update_wnd;
}