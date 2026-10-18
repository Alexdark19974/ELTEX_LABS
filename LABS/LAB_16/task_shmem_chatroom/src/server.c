#include "chatroom_header.h"

void try_wait(sem_t *);

void chatroom_server_init(void)
{
    initscr();
    cbreak();
    noecho();
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLUE);
    wbkgd(stdscr, COLOR_PAIR(1));
    refresh();
    curs_set(TRUE);
    signal(SIGWINCH, sig_winch);

    WINDOW *tab_wnd = init_tab_wnd();
    WINDOW *chit_chat_wnd = init_chit_chat_wnd();
    WINDOW *user_list_wnd = init_user_list_wnd();
    WINDOW *update_wnd = init_update_wnd();

    char *shmmem_name_ptr_1 = strdup("/chatroom_server.c");
    if (NULL == shmmem_name_ptr_1) { error_check_ptr("strdup"); }

    int shmfd_1 = shm_mem_init(shmmem_name_ptr_1);

    char *ipc_data_ptr_1 = mmap(NULL, SEGSIZE, PROT_READ| PROT_WRITE, MAP_SHARED, shmfd_1, 0);
    if (MAP_FAILED == ipc_data_ptr_1) { error_check_ptr("mmap"); }
  
    char *sem_name_1 = strdup("/cleanup.c");
    if (NULL == sem_name_1) { error_check_ptr("strdup"); }

    char *sem_name_2 = strdup("/client.c");
    if (NULL == sem_name_2) { error_check_ptr("strdup"); }

    sem_t *semaphore_1 = sem_open(sem_name_1, O_CREAT | O_EXCL, 0666, 1);
    if (SEM_FAILED == semaphore_1) { error_check_ptr("sem_open"); }

    sem_t *semaphore_2 = sem_open(sem_name_2, O_CREAT | O_EXCL, 0666, 1);
    if (SEM_FAILED == semaphore_2) { error_check_ptr("sem_open"); }

    struct Client_server *wait_refresh_ptr = calloc(1, sizeof(struct Client_server));
    if (NULL == wait_refresh_ptr) { error_check_ptr("calloc"); }

    struct Garbage_collector *gb = calloc(1, sizeof(struct Garbage_collector));
    if (NULL== gb) { error_check_ptr("calloc"); }

    gb->ptr = wait_refresh_ptr;

    wait_refresh_ptr->shm_fd_1 = shmfd_1;
    gb->shm_fd_1 = shmfd_1;

    wait_refresh_ptr->sem_ptr_1 = semaphore_1;
    gb->sem_ptr_1 = semaphore_1;
    gb->sem_name_ptr_1 = sem_name_1;

    wait_refresh_ptr->sem_ptr_2 = semaphore_2;
    gb->sem_ptr_2 = semaphore_2;
    gb->sem_name_ptr_2 = sem_name_2;

    gb->shm_name_1_ptr = shmmem_name_ptr_1;

    wait_refresh_ptr->data_ptr_1 = ipc_data_ptr_1;
    gb->shm_ipc_ptr_1 = ipc_data_ptr_1;

    wait_refresh_ptr->chit_chat_wnd_ptr = chit_chat_wnd;
    wait_refresh_ptr->user_list_wnd_ptr = user_list_wnd;
    wait_refresh_ptr->update_wnd_ptr = update_wnd;
    wait_refresh_ptr->tab_wnd_ptr = tab_wnd;
   
    int serv_pid[2] = {0};

    serv_pid[0] = fork();
    error_check(serv_pid[0], "fork");

    if (0 == serv_pid[0])
    {
        server_control_room(wait_refresh_ptr);
    }

    serv_pid[1] = fork();
    error_check(serv_pid[1], "fork");

    if (0 == serv_pid[1])
    {
        refresh_chat_room(wait_refresh_ptr);
    }

    int state = wait(&serv_pid[0]);
    error_check(state, "wait");

    state = kill(serv_pid[1], SIGTERM);
    error_check(state, "kill");

    cleanup(gb);

    delete_windows(chit_chat_wnd, user_list_wnd, tab_wnd, update_wnd);
}

void refresh_chat_room(struct Client_server *wait_refresh_tmp_ptr)
{
    int state = 0;
  
    while (TRUE)
    {
        state = sem_wait(wait_refresh_tmp_ptr->sem_ptr_1);
        error_check(state, "sem_wait");
        
        if (NULL != strstr(wait_refresh_tmp_ptr->data_ptr_1, "PID"))
        {
            mvwprintw(wait_refresh_tmp_ptr->update_wnd_ptr, 1, 1, "New user!");
        }
        else if (*(wait_refresh_tmp_ptr->data_ptr_1) != '\0')
        {
            mvwprintw(wait_refresh_tmp_ptr->update_wnd_ptr, 1, 1, "Message");
        }
        else
        {
            mvwprintw(wait_refresh_tmp_ptr->update_wnd_ptr, 1, 1, "Waiting...");
        }

        wrefresh(wait_refresh_tmp_ptr->update_wnd_ptr);

        sleep(2);

        state = sem_post(wait_refresh_tmp_ptr->sem_ptr_1);
        error_check(state, "sem_post");

        sleep(2);
    }

    exit(EXIT_SUCCESS);
}

void server_control_room(struct Client_server *wait_refresh_tmp_ptr)
{
    static char pswd[10] = "QWERTY";
    static char data[20] = "some data";
    char *delim = " ";
    char *parsed_user_id[2] = {NULL};
    char *parsed_user_ptr[2] = {NULL};
    int countdown = 0;
    int state = 0;
    int cur_y = 1;
    int cur_x = 1;
    int user_cur_y = 2;
    int yes_pswd = FALSE;
    int resp_flag = FALSE;
    int i = 0;
    int user_counter = 0;
    int user_id_data[100] = {0};

    for (int k = 0; k < 2; k++)
    {
        parsed_user_id[k] = calloc(100, sizeof(char));
        parsed_user_ptr[k] = parsed_user_id[k];
    }

    keypad(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, TRUE);

        while (TRUE)
        {
            if (*wait_refresh_tmp_ptr->data_ptr_1 != '\0')
            {
                try_wait(wait_refresh_tmp_ptr->sem_ptr_1);

                if (NULL != strstr(wait_refresh_tmp_ptr->data_ptr_1, "PID"))
                {
                    resp_flag = TRUE;

                    if (user_cur_y == 1)
                    {
                        user_cur_y++;
                    }

                    parsed_user_id[0] = strtok(wait_refresh_tmp_ptr->data_ptr_1, delim);

                    parsed_user_id[1] = strtok(NULL, delim);

                    user_id_data[user_counter] = atoi(parsed_user_id[1]);
                    user_counter++;

                    mvwprintw(wait_refresh_tmp_ptr->user_list_wnd_ptr, user_cur_y, 1, "user%d (PID: %d)", user_counter, user_id_data[user_counter - 1]);
                    wrefresh(wait_refresh_tmp_ptr->user_list_wnd_ptr);

                    clean_string(parsed_user_id[0]);
                    clean_string(parsed_user_id[1]);
                    clean_string(wait_refresh_tmp_ptr->data_ptr_1);

                    state = sem_post(wait_refresh_tmp_ptr->sem_ptr_2);
                    error_check(state , "sem_post");
                }
                else if (NULL != strstr(wait_refresh_tmp_ptr->data_ptr_1, "request data"))
                {
                    clean_string(wait_refresh_tmp_ptr->data_ptr_1);

                    mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: request processed succesfully.");
                    wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                    cur_y++;

                    sleep(1);
                    
                    mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: send the data to user%d (PID: %d)? y/n", user_counter, user_id_data[user_counter - 1]);
                    wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                    cur_y++;

                    int sym = 0;

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
                                    clean_string(wait_refresh_tmp_ptr->data_ptr_1 + 500);

                                    yes_pswd = TRUE;

                                    strncpy(wait_refresh_tmp_ptr->data_ptr_1 + 500, "password required", strlen("password required") + 1);
                                    (wait_refresh_tmp_ptr->data_ptr_1 + 500)[strlen(wait_refresh_tmp_ptr->data_ptr_1 + 500) + 1] = '\0';

                                    state = sem_post(wait_refresh_tmp_ptr->sem_ptr_2);
                                    error_check(state , "sem_post");

                                    countdown = 9;

                                    while (countdown >= 0)
                                    {
                                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: waiting for the password from the user%d (PID: %d)...%d", user_counter, user_id_data[user_counter - 1], countdown);
                                        wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                                        sleep(1);
                                        countdown--;
                                    }

                                    cur_y++;

                                    if (0 == strncmp(wait_refresh_tmp_ptr->data_ptr_1, pswd, strlen(pswd)))
                                    {
                                        clean_string(wait_refresh_tmp_ptr->data_ptr_1 + 500);

                                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: user%d (PID: %d) entered the correct password", user_counter, user_id_data[user_counter - 1]);

                                        wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                                        cur_y++;

                                        strncpy(wait_refresh_tmp_ptr->data_ptr_1 + 500, "Granted", strlen("Granted") + 1);
                                        (wait_refresh_tmp_ptr->data_ptr_1 + 500)[strlen(wait_refresh_tmp_ptr->data_ptr_1 + 500) + 1] = '\0';

                                        state = sem_post(wait_refresh_tmp_ptr->sem_ptr_2);
                                        error_check(state , "sem_post");
                                    }
                                    else
                                    {
                                        clean_string(wait_refresh_tmp_ptr->data_ptr_1 + 500);

                                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: user%d (PID: %d) entered the wrong password or timed out", user_counter, user_id_data[user_counter - 1]);
                                        wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                                        cur_y++;

                                        strncpy(wait_refresh_tmp_ptr->data_ptr_1 + 500, "Rejected", strlen("Rejected") + 1);
                                        (wait_refresh_tmp_ptr->data_ptr_1 + 500)[strlen(wait_refresh_tmp_ptr->data_ptr_1 + 500) + 1] = '\0';
                                       
                                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: rejected user%d (PID: %d)", user_counter, user_id_data[user_counter - 1]);
                                        wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                                        state = sem_post(wait_refresh_tmp_ptr->sem_ptr_2);
                                        error_check(state , "sem_post");

                                        resp_flag = TRUE;
                                    }

                                    break;
                                } 
                                case 'n':
                                {
                                    clean_string(wait_refresh_tmp_ptr->data_ptr_1 + 500);

                                    yes_pswd = TRUE;

                                    strncpy(wait_refresh_tmp_ptr->data_ptr_1 + 500, "Granted", strlen("Granted") + 1);
                                    (wait_refresh_tmp_ptr->data_ptr_1 + 500)[strlen(wait_refresh_tmp_ptr->data_ptr_1 + 500) + 1] = '\0';

                                    resp_flag = TRUE;

                                    state = sem_post(wait_refresh_tmp_ptr->sem_ptr_2);
                                    error_check(state , "sem_post");
                           
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
                            clean_string(wait_refresh_tmp_ptr->data_ptr_1 + 500);

                            mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: rejected user%d (PID: %d)", user_counter, user_id_data[user_counter - 1]);
                            wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                            cur_y++;

                            strncpy(wait_refresh_tmp_ptr->data_ptr_1 + 500, "Rejected", strlen("Rejected") + 1);
                            (wait_refresh_tmp_ptr->data_ptr_1 + 500)[strlen(wait_refresh_tmp_ptr->data_ptr_1 + 500) + 1] = '\0';

                            state = sem_post(wait_refresh_tmp_ptr->sem_ptr_2);
                            error_check(state , "sem_post");
                            resp_flag = TRUE;
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }

                    if (TRUE == yes_pswd)
                    {
                        mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: sending the requested data to user%d (PID: %d)", user_counter, user_id_data[user_counter - 1]);
                        wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                        cur_y++;

                        sleep(5);

                        clean_string(wait_refresh_tmp_ptr->data_ptr_1 + 500);

                        strncpy(wait_refresh_tmp_ptr->data_ptr_1 + 500, data, strlen(data) + 1);
                        (wait_refresh_tmp_ptr->data_ptr_1 + 500)[strlen(data) + 1] = '\0';

                        state = sem_post(wait_refresh_tmp_ptr->sem_ptr_2);
                        error_check(state , "sem_post");

                        countdown = 5;
                        while (countdown >= 0)
                        {
                            mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "%s Admin: waiting for the response from the user%d (PID: %d)...%d", wait_refresh_tmp_ptr->data_ptr_1, user_counter, user_id_data[user_counter - 1], countdown);
                            wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                            sleep(1);

                            countdown--;
                        }

                        cur_y++;

                        if (0 == strcmp(wait_refresh_tmp_ptr->data_ptr_1, "Success"))
                        {
                            mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: succesfully received %s from user%d (PID: %d)", wait_refresh_tmp_ptr->data_ptr_1, user_counter, user_id_data[user_counter - 1]);
                            wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                            cur_y++;
                        }

                        resp_flag = TRUE;
                        yes_pswd = FALSE;

                        clean_string(wait_refresh_tmp_ptr->data_ptr_1);
                        clean_string(wait_refresh_tmp_ptr->data_ptr_1 + 500);
                    }
                }
                else if (NULL != strstr(wait_refresh_tmp_ptr->data_ptr_1, "disconnect"))
                {
                    mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "User%d: %s", user_counter, wait_refresh_tmp_ptr->data_ptr_1);
                    wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                    cur_y++;

                    mvwprintw(wait_refresh_tmp_ptr->user_list_wnd_ptr, user_cur_y, 1, "                     ");

                    mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "Admin: user%d (PID: %d) left the chat", user_counter, user_id_data[user_counter - 1]);
                    wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                    wrefresh(wait_refresh_tmp_ptr->user_list_wnd_ptr);

                    clean_string(wait_refresh_tmp_ptr->data_ptr_1);

                    user_counter--;
                    user_id_data[user_counter] = 0;
                    cur_y++;

                    if (user_cur_y > 1)
                    {
                        user_cur_y--;
                    }
                }
                else 
                {
                    wattroff(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, A_BLINK);

                    mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, 1, "User%d: %s", user_counter, wait_refresh_tmp_ptr->data_ptr_1);
                    wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                    cur_y++;
                    cur_x = 1;
                    i++;

                    sleep(1);

                    clean_string(wait_refresh_tmp_ptr->data_ptr_1);
                }

                if (FALSE == resp_flag)
                {
                    echo();
                    curs_set(TRUE);

                    char buf[100] = {0};

                    mvwprintw(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, cur_x, "Admin: ");
                    wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);
                    cur_x = 7;

                    wgetnstr(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, buf, 100);
                    cur_y++;
                    cur_x = 1;

                    wmove(wait_refresh_tmp_ptr->chit_chat_wnd_ptr, cur_y, cur_x);
                    wrefresh(wait_refresh_tmp_ptr->chit_chat_wnd_ptr);

                    if (buf[0] == 'q' && buf[1] == '\0')
                    {
                        strncpy(wait_refresh_tmp_ptr->data_ptr_1 + 500, "server died", strlen("server died") + 1);
                        (wait_refresh_tmp_ptr->data_ptr_1 + 500)[strlen(wait_refresh_tmp_ptr->data_ptr_1 + 500) + 1] = '\0';

                        for (int k = 0; k < 2; k++)
                        {
                            free(parsed_user_ptr[k]);
                            parsed_user_id[k] = NULL;
                        }

                        state = sem_post(wait_refresh_tmp_ptr->sem_ptr_2);
                        error_check(state, "sem_post");

                        exit(EXIT_SUCCESS);
                    }

                    strncpy(wait_refresh_tmp_ptr->data_ptr_1 + 500, buf, strlen(buf) + 1);
                    (wait_refresh_tmp_ptr->data_ptr_1 + 500)[strlen(wait_refresh_tmp_ptr->data_ptr_1 + 500) + 1] = '\0';

                    state = sem_post(wait_refresh_tmp_ptr->sem_ptr_2);
                    error_check(state, "sem_post");

                    clean_string(wait_refresh_tmp_ptr->data_ptr_1);
                } 

            resp_flag = FALSE;

            sleep(2);

            state = sem_post(wait_refresh_tmp_ptr->sem_ptr_1);
            error_check(state, "sem_post");
        }
    }
}

void clean_string(char *buffer_to_be_cleaned)
{
    for (int j = 0; buffer_to_be_cleaned[j] != '\0'; j++)
    {
        buffer_to_be_cleaned[j] = '\0';
    }
}

void try_wait(sem_t *sem_ptr_1)
{
    int sval = 0;
    int state = 0;

    while (TRUE)
    {
        state = sem_getvalue(sem_ptr_1, &sval);
        error_check(state, "sem_getvalue");

        if (sval == 1)
        {
            sem_wait(sem_ptr_1);
            break;   
        }
    }
}
