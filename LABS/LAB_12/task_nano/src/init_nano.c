#include "nano.h"

int init_nano(nano_s *self)
{
    int i = 0;

    memset(self, 0x0, sizeof(nano_s));
   
    if (NULL == initscr()) {
        snprintf(self->err_buf, ERR_BUFSIZ, "[%s:%d] error: failed to open stdscr (%s)", __func__, __LINE__, strerror(errno));
        return FAIL;
    }

    self->m_y = LINES; // line number; vertical postion
    self->m_x = COLS;  // position within line; horizontal position

    self->lin_buf = (char **) calloc(self->m_y - 5, sizeof(char *)); // alloc pointers to rows  
    if (NULL == self->lin_buf) {
        snprintf(self->err_buf, ERR_BUFSIZ, "[%s:%d] error: calloc failed! (%s)", __func__, __LINE__, strerror(errno));
        return FAIL;
    }

    for (i = 0; i < self->m_y - 5; i++) {
        self->lin_buf[i] = calloc(1, sizeof(char) * (self->m_x - 1)); // alloc cells for each row to make a column
        if (NULL == self->lin_buf[i]) {
            snprintf(self->err_buf, ERR_BUFSIZ, "[%s:%d] error: calloc failed! (%s)", __func__, __LINE__, strerror(errno));
            return FAIL;
        }
    }

    self->menu_wnd = derwin(stdscr, /* lines */ 4, /* cols */ self->m_x - 2, /* relative to stdscr */ self->m_y - 5, 1);
    if (NULL == self->menu_wnd) {
        snprintf(self->err_buf, ERR_BUFSIZ, "[%s:%d] error: failed to open menu_wnd (%s)", __func__, __LINE__, strerror(errno));
        return FAIL;
    }

#ifdef DEBUG
    self->display_xy = display_xy;
#endif
    self->redraw = redraw;

    cbreak();
    keypad(stdscr, TRUE);
    mousemask(BUTTON1_CLICKED, NULL);
    curs_set(TRUE);
    noecho();

    return OK;
}