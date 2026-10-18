#include "nano.h"

void deinit_nano(nano_s *self)
{
    int i = 0;

    if (self->lin_buf) {
        for (i = 0; i < self->m_y - 5; i++) {
            if (self->lin_buf[i])
                free(self->lin_buf[i]);
        }
        free(self->lin_buf);
    }

    delwin(self->menu_wnd); // remove the menu win    
    endwin(); // remove the main win
}
