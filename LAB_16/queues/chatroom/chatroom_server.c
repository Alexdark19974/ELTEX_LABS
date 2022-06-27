#include "chatroom_header.h"

static char msg[MSG_MAX] = {0};
static void clean_string(char *);

void chatroom_server_init(void)
{
    initscr();
    cbreak();
    noecho();
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLUE);
    wbkgd(stdscr, COLOR_PAIR(1));
    refresh();
    signal(SIGWINCH, sig_winch);

    WINDOW *tab_wnd = init_tab_wnd();
    WINDOW *chit_chat_wnd = init_chit_chat_wnd();
    WINDOW *user_list_wnd = init_user_list_wnd();

    char *msg_q_name = strdup("/session_queue");

    if (NULL == msg_q_name)
    {
        handle_error("strdup");
    }

    char *msg_q_name_2 = strdup("/special_queue");

    if (NULL == msg_q_name_2)
    {
        handle_error("strdup");
    }
    char *buffer = calloc(MSG_MAX, sizeof(char));
    int qfd = message_queue_init(msg_q_name);
    int qfd_2 = message_queue_init(msg_q_name_2);
    int user_pid = 0;
    int state = 0;
    struct Threads *wait_refresh_ptr = calloc(THREAD_MAX, sizeof(struct Threads));

    if (NULL == wait_refresh_ptr)
    {
        handle_error("calloc");
    }

    for (int i = 0; i < THREAD_MAX; i++)
    {
        wait_refresh_ptr[i].qu_fd = qfd;
        wait_refresh_ptr[i].qu_fd_2 = qfd_2;
        wait_refresh_ptr[i].chit_chat_wnd_ptr = chit_chat_wnd;
        wait_refresh_ptr[i].buffer_ptr = buffer;
        wait_refresh_ptr[i].user_pid_ptr = &user_pid;
        wait_refresh_ptr[i].user_list_wnd_ptr = user_list_wnd;
        wait_refresh_ptr[i].tab_wnd_ptr = tab_wnd;
    }

    for (int i = 0; i < THREAD_MAX; i++)
    {
        if (1 == i)
        {
            state = pthread_create(&wait_refresh_ptr[i].thread_id, NULL, &server_control_room, &wait_refresh_ptr[i]);

            if (-1 == state)
            {
                handle_error_en(state, "pthread_create");
            }
        }
        else
        {
            state = pthread_create(&wait_refresh_ptr[i].thread_id, NULL, &refresh_chat_room, &wait_refresh_ptr[i]);

            if (-1 == state)
            {
                handle_error_en(state, "pthread_create");
            }
        }
    }

    for (int i = 0; i < THREAD_MAX; i++)
    {
        state = pthread_join(wait_refresh_ptr[i].thread_id, NULL);

        if (-1 == state)
        {
            handle_error_en(state, "pthread_join");
        }
    }

    cleanup(qfd, msg_q_name, msg_q_name_2, wait_refresh_ptr, buffer);

    delete_windows(chit_chat_wnd, user_list_wnd, tab_wnd);
}

void *refresh_chat_room(void *const args)
{
    int state = 0;
    unsigned int prio = 0;
    struct Threads *wait_refresh_tmp_ptr = args;
    while (TRUE)
    {
        state = mq_receive(wait_refresh_tmp_ptr->qu_fd_2, wait_refresh_tmp_ptr->buffer_ptr, MSG_MAX, &prio);
         
        if (0 == strcmp(wait_refresh_tmp_ptr->buffer_ptr, "kill") || -1 == state)
        {
            break;
        }

        if (2 == prio)
        {

            *wait_refresh_tmp_ptr->user_pid_ptr = atoi(wait_refresh_tmp_ptr->buffer_ptr);
            strncpy(msg, "PID", strlen("PID"));
            msg[(strlen("PID") + 1)] = '\0';
            clean_string(wait_refresh_tmp_ptr->buffer_ptr);
            continue;
        }

        int buflen = strlen(wait_refresh_tmp_ptr->buffer_ptr);

        strncpy(msg, wait_refresh_tmp_ptr->buffer_ptr, buflen);
        msg[buflen] = '\0';

        clean_string(wait_refresh_tmp_ptr->buffer_ptr);
    }

    pthread_exit(NULL);
}

void *server_control_room(void *const args)
{
    struct Threads *wait_refresh_tmp_ptr = args;
    static char pswd[MSG_MAX] = "QWERTY";
    static char data[MSG_MAX] = "some data";
    int state = 0;
    int cur_y = 1;
    int cur_x = 1;
    int user_cur_y = 2;
    int max_y = 0;
    int max_x = 0;
    int no_pswd = FALSE;
    int i = 0;
    int user_counter = 1;
    int user_id_data[MSG_MAX] = {0};

    getmaxyx(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, max_y, max_x);
    max_y = max_y / 2;
    max_x = max_x - 100; /* used to calculate the center of the screen */
    keypad(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, TRUE);

    while (TRUE)
    {
        curs_set(FALSE);
        wattron(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, A_BLINK);
        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, max_y, max_x, "Press 'F4' to get a list of options");
        wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

        int sym = 0;

        while ((sym = wgetch(wait_refresh_tmp_ptr->chit_chat_wnd_ptr)))
        {
            mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, max_y, max_x, "                                                        ");
        
            switch (sym)
            {
                case KEY_F(1):
                {
                    mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, max_y, max_x, "checking if there are new users...");
                    wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                    sleep(1);

                    if (msg[0] != '\0')
                    {
                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, max_y, max_x, "                                                        ");

                        if (0 == strncmp(msg, "PID", 3))
                        {
                            if (user_cur_y == 1)
                            {
                                user_cur_y++;
                            }

                            mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, max_y, max_x, "user entered the chat.");
                            wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                            sleep(3);

                            mvwprintw(wait_refresh_tmp_ptr->user_list_wnd_ptr, user_cur_y, 1, "user%d (PID: %d)", user_counter, *wait_refresh_tmp_ptr->user_pid_ptr);

                            wrefresh(wait_refresh_tmp_ptr->user_list_wnd_ptr);

                            for (int m = 0; user_id_data[m] != '\0'; i++)
                            {
                                if (user_id_data[m] != *wait_refresh_tmp_ptr->user_pid_ptr)
                                {
                                    user_id_data[m] = *wait_refresh_tmp_ptr->user_pid_ptr;
                                    user_cur_y++;
                                    break;
                                }
                                else
                                {
                                    user_cur_y--;
                                }
                            }

                            clean_string(msg);
                            mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, max_y, max_x, "                                                        ");
                            continue;
                        }
                    }
                    else
                    {
                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, max_y, max_x, "                                                        ");
                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, max_y, max_x, "             no users.");
                        
                        wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                        sleep(2);

                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, max_y, max_x, "                                                        ");
                        continue;
                   }

                    mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, max_y, max_x, "                                   ");

                    if (user_cur_y == 1)
                    {
                        user_cur_y++;
                    }

                    wattroff(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, A_BLINK);
                    mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "User%d: %s", user_counter, msg);

                    wrefresh(wait_refresh_tmp_ptr->user_list_wnd_ptr);
                    wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                    cur_y++;

                    i++;

                    sleep(2);

                    if (NULL != strstr(msg, "request data"))
                    {
                        clean_string(wait_refresh_tmp_ptr->buffer_ptr);

                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: request processed succesfully.");
                        wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                        cur_y++;

                        sleep(1);
                        
                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: send the data to user%d (PID: %d)? y/n", user_counter, *wait_refresh_tmp_ptr->user_pid_ptr);
                        wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                        cur_y++;

                        switch (sym = tolower(getchar()))
                        {
                            case 'y':
                            {
                                mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: require the password? y/n");
                                wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                                cur_y++;
                                                      
                                switch (sym = tolower(getchar()))
                                {
                                    case 'y':
                                    {
                                        char req[MSG_MAX] = "password required";

                                        state = mq_send(wait_refresh_tmp_ptr->qu_fd, req, MSG_MAX, 1); 

                                        if (-1 == state)
                                        {
                                            handle_error_en(state, "mq_send");
                                        }

                                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: waiting for the password from the user%d (PID: %d)...", user_counter, *wait_refresh_tmp_ptr->user_pid_ptr);
                                        wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                                        cur_y++;

                                        sleep(15);

                                        if (0 == strncmp(msg, pswd, strlen(pswd)))
                                        {
                                            mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: user%d (PID: %d) entered the correct password", user_counter, *wait_refresh_tmp_ptr->user_pid_ptr);
                                            clean_string(msg);

                                            wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                                            cur_y++;
                                            char message[MSG_MAX] = "Granted";
                                            state = mq_send(wait_refresh_tmp_ptr->qu_fd, message, MSG_MAX, 1);

                                            if (-1 == state)
                                            {
                                                handle_error_en(state, "mq_send");
                                            }

                                            sleep(5);
                                        }
                                        else
                                        {
                                            mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: user%d (PID: %d) entered the wrong password or timed out", user_counter, *wait_refresh_tmp_ptr->user_pid_ptr);
                                            wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                                            cur_y++;

                                            char reject[MSG_MAX] = "rejected";
                                            state = mq_send(wait_refresh_tmp_ptr->qu_fd, reject, MSG_MAX, 1);

                                            if (-1 == state)
                                            {
                                                handle_error_en(state, "mq_send");
                                            }
                                     
                                            mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: rejected user%d (PID: %d)", user_counter, *wait_refresh_tmp_ptr->user_pid_ptr);
                                            wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                                                                                      
                                            sleep(5);
                                            continue;
                                        }

                                        break;
                                    } 
                                    case 'n':
                                    {
                                        no_pswd = TRUE;
                                        break;
                                    }
                                    default:
                                    {
                                        break;
                                    }
                                }
                                break;
                            }
                            case 'n':
                            {
                                char reject[MSG_MAX] = "rejected";
                                state = mq_send(wait_refresh_tmp_ptr->qu_fd , reject, MSG_MAX, 1);

                                if (-1 == state)
                                {
                                    handle_error_en(state, "mq_send");
                                }

                                mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: rejected user%d (PID: %d)", user_counter, *wait_refresh_tmp_ptr->user_pid_ptr);
                                wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                                cur_y++;

                                sleep(5);

                                clean_string(msg);

                                for (int j = 0; wait_refresh_tmp_ptr->buffer_ptr[j] != '\0'; j++)
                                {
                                    wait_refresh_tmp_ptr->buffer_ptr[j] = '\0';
                                }

                                wattron(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, A_BLINK);
                                mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, max_y, max_x, "Press 'F4' to get a list of options");
                                wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                                wattroff(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, A_BLINK);
                                continue;
                            }
                            default:
                            {
                                break;
                            }
                        }

                        if (TRUE == no_pswd)
                        {   
                            char message[MSG_MAX] = "Granted";
                            mq_send(wait_refresh_tmp_ptr->qu_fd, message, MSG_MAX, 1);

                            if (-1 == state)
                            {
                                handle_error_en(state, "mq_send");
                            }

                            sleep(5);
                            no_pswd = FALSE;
                        }

                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: sending the requested data to user%d (PID: %d)", user_counter, *wait_refresh_tmp_ptr->user_pid_ptr);
                        wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                        cur_y++;

                        state = mq_send(wait_refresh_tmp_ptr->qu_fd, data, MSG_MAX, 1);

                        if (-1 == state)
                        {
                                handle_error_en(state, "mq_send");
                        }

                        wrefresh(wait_refresh_tmp_ptr->user_list_wnd_ptr);

                        sleep(5);

                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: waiting for the response from the user%d (PID: %d)...", user_counter, *wait_refresh_tmp_ptr->user_pid_ptr);
                        wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                        cur_y++;

                        sleep(5);

                        if (0 == strcmp(msg, "Success"))
                        {
                            mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: succesfully received %s from user%d (PID: %d)", msg, user_counter, *wait_refresh_tmp_ptr->user_pid_ptr);
                            wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                            cur_y++;
                        }
                        else
                        {
                            mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin no respone. Timed out.");
                            wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                            cur_y++;
                        }
                    }
                    else 
                    {
                        if (NULL != strstr(msg, "disconnect"))
                        {
                            mvwprintw(wait_refresh_tmp_ptr->user_list_wnd_ptr, user_cur_y, 1, "                     ", user_counter, *wait_refresh_tmp_ptr->user_pid_ptr);
                            mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: user%d (PID: %d) left the chat", user_counter, *wait_refresh_tmp_ptr->user_pid_ptr);
                            wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                            wrefresh(wait_refresh_tmp_ptr->user_list_wnd_ptr);
                            cur_y++;
                        }
                        else
                        {
                            curs_set(TRUE);
                            echo();

                            wattroff(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, A_BLINK);
                            
                            char resp[MSG_MAX] = {0};

                            mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: ");
                            wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                            cur_x+=7;

                            wmove(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, cur_x);
                            wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                            wgetnstr(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, resp, MSG_MAX);

                            cur_y++;
                            cur_x = 1;

                            mq_send(wait_refresh_tmp_ptr->qu_fd, resp, MSG_MAX, 1);

                            if (-1 == state)
                            {
                                handle_error_en(state, "mq_send");
                            }

                            curs_set(FALSE);

                            sleep(5);
                        }
                    }

                    wattron(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, A_BLINK);
                    wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                    break;
                }
                case KEY_F(2):
                {
                    echo();
                    curs_set(TRUE);

                    wattroff(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, A_BLINK);

                    char buf[MSG_MAX] = {0};

                    mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, cur_x, "Admin: ");
                    wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                    cur_x+= 7;

                    wmove(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, cur_x);
                    wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                    while (TRUE)
                    {
                        wgetnstr(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, buf, 100);

                        if (NULL != strstr(buf, "exit chat"))
                        {
                            cur_y++;
                            cur_x = 1;
                            wmove(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, cur_x);
                            wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                            break;
                        }

                        cur_y++;
                        cur_x = 1;

                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, cur_x, "Admin: ");
                        wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                        cur_x = 8;
                    }

                    break;
                }   
                case KEY_F(3):
                {
                    clean_string(msg);
                    clean_string(wait_refresh_tmp_ptr->buffer_ptr);

                    mq_send(wait_refresh_tmp_ptr->qu_fd_2, "kill", 4, 1);

                    if (-1 == state)
                    {
                        handle_error_en(state, "mq_send");
                    }

                    mq_close(wait_refresh_tmp_ptr->qu_fd_2);

                    if (-1 == state)
                    {
                        handle_error_en(state, "mq_close");
                    }

                    pthread_exit(NULL);
                }
                case KEY_F(4):
                {
                    int tab_beg_y = 0;
                    int tab_beg_x = 0;
                    getbegyx(wait_refresh_tmp_ptr->tab_wnd_ptr, tab_beg_y, tab_beg_x);

                    tab_beg_y--; 
                    tab_beg_x+=12;

                    wattron(wait_refresh_tmp_ptr->tab_wnd_ptr, A_BOLD);
                    mvwprintw(wait_refresh_tmp_ptr->tab_wnd_ptr, tab_beg_y, tab_beg_x, "F1 - refresh  |  F2 - start chatting  |  F3 - exit the chatroom | F4 - get a list of options");
                    wrefresh(wait_refresh_tmp_ptr->tab_wnd_ptr);     

                    break;
                }
                default:
                {
                    break;
                }
            }

            clean_string(msg);
            clean_string(wait_refresh_tmp_ptr->buffer_ptr);
            break;
        }
    }

    clean_string(wait_refresh_tmp_ptr->buffer_ptr);

    state = mq_send(wait_refresh_tmp_ptr->qu_fd_2, "kill", 4, 1);

    if (-1 == state)
    {
        handle_error_en(state, "mq_send");
    }

    mq_close(wait_refresh_tmp_ptr->qu_fd_2);

    if (-1 == state)
    {
        handle_error_en(state, "mq_close");
    }

    pthread_exit(NULL);
}

void clean_string(char *buffer_to_be_cleaned)
{
    for (int j = 0; buffer_to_be_cleaned[j] != '\0'; j++)
    {
        buffer_to_be_cleaned[j] = '\0';
    }
}
