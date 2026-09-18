#include "nano.h"

nano_s nano;

int nano_editor(int argc, char **argv)
{
    // FILE *fp = NULL;
    char *cur_str = NULL, *next_str = NULL;
    // MEVENT event;
    int ret = EXIT_SUCCESS, /*i = 0,*/ lock = 0;
    size_t /*cur_str_len = 0,*/ next_str_len = 0;
    bool loop = true;
    char *ch_p = NULL, cur_ch = 0;

    if (FAIL == init_nano(&nano))
        goto err;

    nano.redraw(&nano, STDSCR);

    {
        attron(A_REVERSE);    
        mvprintw(0, 3, DISPLAY_EDITOR_NAME);
        if (argc < 2)
            mvprintw(0, nano.m_x / 2, DISPLAY_NEW_BUFFER);    
        attroff(A_REVERSE);
    }

    (void)argv;

    nano.redraw(&nano, MENUSCR);
    
    nano.c_y = BEG_Y, nano.c_x = BEG_X;
    move(nano.c_y, nano.c_x);

    while (loop) {
        switch ((nano.ch = getch())) {
            case KEY_MOUSE:
                {
                    move(nano.c_y, nano.c_x);
                }
                break;
            case KEY_F(2):
                {
                    nano.redraw(&nano, MENUSCR);
                }
            /* fall through */
            case KEY_F(4):
                {
                    attron(A_BLINK | A_BOLD);
                    curs_set(FALSE);
                    clear();
                    mvprintw((nano.m_y / 2), (nano.m_x / 2) - (strlen(DISPLAY_PRESS_ANY_KEY) / 2), DISPLAY_PRESS_ANY_KEY);
                    getch();
                    loop = 0;
                }
                break;
            case KEY_UP:
                {
                    if (nano.c_y > UPPER_BORDER) {
                        // cur_str = nano.lin_buf[nano.c_y - 1];   // [1]  
                        next_str = nano.lin_buf[nano.c_y - 2];  // [0]
                        // cur_str_len = strlen(cur_str);
                        next_str_len = strlen(next_str);

                        nano.c_x = nano.max_x;
                        if (IS_EMPTY(next_str[nano.c_x - 1])) // is next string at specific pos empty?
                            nano.c_x = next_str_len; // adjust by next string's length
                        move(--nano.c_y, nano.c_x);
                    }
                }
                break;
            case KEY_DOWN:
                {
                    if (nano.c_y < MENU_BORDER) {
                        cur_str = nano.lin_buf[nano.c_y - 1];   // [0]
                        next_str = nano.lin_buf[nano.c_y];      // [1]
                        // cur_str_len = strlen(cur_str); // calculate current string's length
                        next_str_len = strlen(next_str); // calculate next string's length
                        if (!strchr(next_str, NEWLINE)) // account for missing '\n'
                            ++next_str_len;

                        if (IS_EMPTY(next_str[BEG_X])) { // next string is empty
                            if (!strchr(cur_str, NEWLINE)) // no clue that we're allowed to move cursor to [][BEG_X] based on current line;
                                break;
                            else
                                nano.c_x = BEG_X; // '\n' tells us we should jump to a new line
                        } else { // next string contains something
                            /* default x pos is previous max x pos */
                            nano.c_x = nano.max_x;
                            /* if cursor pos is larger than the next string's length, adjust by its length, otherwise use previous max row pos */
                            if (nano.c_x > (int)next_str_len)
                                nano.c_x = next_str_len;
                        }
                        move(++nano.c_y, nano.c_x);
                    }
                }
                break;
            case KEY_LEFT:
                {
                    if (nano.c_x > LEFT_BORDER) {
                        move(nano.c_y, --nano.c_x);
                        nano.max_x = nano.c_x;
                    }
                }
                break;
            case KEY_RIGHT:
                {
                    if (nano.c_x < RIGHT_BORDER) {
                        cur_ch = nano.lin_buf[nano.c_y - 1][nano.c_x - 1];
                        if (IS_EMPTY(cur_ch) || IS_NEWLINE(cur_ch))
                            break;
                        move(nano.c_y, ++nano.c_x);
                        nano.max_x = nano.c_x;
                    }
                }
                break;
            case KEY_BACKSPACE:
                {
                    if (nano.c_x > LEFT_BORDER) {
                        mvaddch(nano.c_y, nano.c_x - 1, ' ');
                        nano.c_x = getcurx(stdscr);
                        move(nano.c_y, --nano.c_x);
                        nano.max_x = nano.c_x;
                        nano.lin_buf[nano.c_y - 1][nano.c_x - 1] = '\0';
                    } else {
                        if (nano.c_y > UPPER_BORDER) {
                            // cur_ch = nano.lin_buf[nano.c_y - 1][nano.c_x - 1];
                            next_str = nano.lin_buf[nano.c_y - 2];
                            // if (cur_ch == '\0' || cur_ch == '\n') {
                            if (next_str[BEG_X]) {
                                next_str_len = strlen(next_str);
                                ch_p = strchr(next_str, NEWLINE);
                                if (ch_p)
                                    *ch_p = '\0';
                                nano.c_x = next_str_len;
                            }
                            // }
                            move(--nano.c_y, nano.c_x);
                            nano.max_x = nano.c_x;
                        }
                    }
                }
                break;
           default:
                {
                    echochar(nano.ch);
                    cur_ch = nano.ch & A_CHARTEXT;
                    nano.lin_buf[nano.c_y - 1][nano.c_x - 1] = cur_ch;
                    nano.c_x = nano.max_x = getcurx(stdscr);
                    if (IS_NEWLINE(cur_ch)) {
                        nano.c_y = getcury(stdscr);
                        nano.c_x = nano.max_x = BEG_X, lock = 0;
                        nano.redraw(&nano, STDSCR);
                        move(nano.c_y, nano.c_x);
                    }
                    if (!lock) {
                        attron(A_REVERSE);
                        mvprintw(0, 3, DISPLAY_EDITOR_NAME);
                        mvprintw(0, nano.m_x / 2, DISPLAY_MODIFIED);
                        move(nano.c_y, nano.c_x);
                        attroff(A_REVERSE);
                        lock = 1;
                    }
                }
                break;
        }

#ifdef DEBUG
        {
            nano.display_xy(&nano);
        }
#endif  
    }

err:
    deinit_nano(&nano);

    if (*nano.err_buf) {
        fprintf(stderr, "%s\n", nano.err_buf);
        ret = EXIT_FAILURE;
    }

    return ret;
}
