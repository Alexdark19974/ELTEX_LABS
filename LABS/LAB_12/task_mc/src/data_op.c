#include "midc.h"

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
