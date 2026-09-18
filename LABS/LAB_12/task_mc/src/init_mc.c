#include "midc.h"

int init_mc(mc_s *self)
{
    int i = 0;

    memset(self, 0x0, sizeof(mc_s));

    for (i = 0; i < WIN_MAX; i++) {
        if (!getcwd(self->path_way[i], sizeof(self->path_way[i])))\
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