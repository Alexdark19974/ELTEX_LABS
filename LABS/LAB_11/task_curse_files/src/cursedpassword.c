#include "common.h"

void get_password(WINDOW *, char *, int);

int main(void)
{
	char password[MAX_LEN] = {0};

	WINDOW *wnd = NULL;

	initscr();

	signal(SIGWINCH, sig_handler);

	curs_set(TRUE);

	start_color();

	refresh();

	init_pair(1, COLOR_YELLOW, COLOR_WHITE);

	wnd = newwin(5, 23, 2, 2);

	wbkgd(wnd, COLOR_PAIR(1));

	wattron(wnd, A_BOLD);

	keypad(wnd, TRUE);

	wprintw(wnd, "Enter your password...\n");

	get_password(wnd, password, MAX_LEN);

	wattron(wnd, A_BLINK);

	if (strcmp(password, RIGHT_PASSWORD) == 0)
	{
		wprintw(wnd, "ACCESS GRANTED!");
	}
	else
	{
		wprintw(wnd, "ACCESS DENIED!");
	}

	wrefresh(wnd);

	wattroff(wnd, A_BOLD | A_BLINK);

	getch();

	endwin();

	exit(EXIT_SUCCESS);
}


void get_password(WINDOW *win, char *password, int max_len)
{
	noecho();
	int i = 0;
	int ch = 0;

	while (((ch = wgetch(win)) != '\n') && (i < max_len - 1))
	{
		if (ch == KEY_BACKSPACE)
		{
			int x = 0;
			int y = 0;

			if (i == 0)
			{
				continue;
			}

			getyx(win, y, x);

			mvwaddch(win, y, x - 1, ' ');

			wrefresh(win);

			wmove(win, y, x - 1);

			i--;

			continue;
		}

		password[i++] = ch;
		wechochar(win, '*');
	}

	password[i] = '\0';
	wechochar(win, '\n');
}

