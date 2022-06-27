FILE_1 = main
FILE_2 = cleanup
FILE_3 = chatroom_server
FILE_4 = delete_windows
FILE_5 = sig_winch
FILE_6 = windows_init
FILE_7 = queue_init
FILE_8 = client
HEADER = chatroom_header
FLAG_1 = -o
FLAG_2 = -Wall
FLAG_3 = -Wshadow
FLAG_4 = -Wextra
FLAG_5 = -pedantic
FLAG_6 = -g
FLAG_7 = -lrt
FLAG_8 = -lpthread
FLAG_9 = -lncurses
EXEC_FILE = chatroom
EXEC_FILE_2 = client

PHONY: all clean chatroom client

all:  $(FILE_1).c $(FILE_2).c $(FILE_3).c $(FILE_4).c $(FILE_5).c $(FILE_6).c $(FILE_7).c $(HEADER).h

		$(CC) $(FILE_1).c $(FILE_2).c $(FILE_3).c $(FILE_4).c $(FILE_5).c $(FILE_6).c $(FILE_7).c $(FLAG_1) $(EXEC_FILE) $(FLAG_2) $(FLAG_3) $(FLAG_4) $(FLAG_5) $(FLAG_6) $(FLAG_7) $(FLAG_8) $(FLAG_9)

		$(CC) $(FILE_8).c $(FLAG_1) $(EXEC_FILE_2) $(FLAG_2) $(FLAG_3) $(FLAG_4) $(FLAG_5) $(FLAG_6) $(FLAG_7)

Chatroom:

		./$(EXEC_FILE)

Client:

		./$(EXEC_FILE_2)
clean:

		rm -rf  $(EXEC_FILE)
		rm -rf $(EXEC_FILE_2)
