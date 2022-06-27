#include "chatroom_header.h"

void delete_windows(WINDOW *chit_chat_wnd, WINDOW *user_list_wnd, WINDOW *tab_wnd)
{
    wclear(chit_chat_wnd);
    wclear(user_list_wnd);
    wclear(tab_wnd);

    delwin(chit_chat_wnd);
    chit_chat_wnd = NULL;
    delwin(user_list_wnd);
    user_list_wnd = NULL;
    delwin(tab_wnd);
    tab_wnd = NULL;
    endwin();
}

